#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#include <cinttypes>

#include "mpegts/tsparser.h"
#include "flv/flvparser.h"
#include "inctime.h"

typedef struct
{
	const char *pVideoOut;
	FILE *pVFile;
	const char *pAudioOut;
	FILE *pAFile;
} CmdArg;
CmdArg arg;
LinkIncTime inctime;

static void usage(int argc, char **argv)
{
    printf("usage as:%s {flv|ts} [options] tsfilename", argv[0]);
	printf("options are:");
	printf("\t[-af afileout] audio stream out\n");
	printf("\t[-vf vfileout] video stream out\n");
}

static void writeFrame(TsParsedFrame *pFrame)
{
	switch (pFrame->stype)
	{
	case TsTypeH264:
	case TsTypeH265:
		if (arg.pVFile == NULL)
		{
			arg.pVFile = fopen(arg.pVideoOut, "w");
		}
		if (arg.pVFile)
		{
			fwrite(pFrame->pData, 1, pFrame->nDataLen, arg.pVFile);
		}
		printf("inctime vpts--->:%" PRId64 "\n", GetVideoPts(&inctime, pFrame->nPts));
		break;

	case TsTypePrivate:
	case TsTypeAAC:
		if (arg.pAFile == NULL)
		{
			arg.pAFile = fopen(arg.pAudioOut, "w");
		}
		if (arg.pAFile)
		{
			fwrite(pFrame->pData, 1, pFrame->nDataLen, arg.pAFile);
		}
		printf("inctime apts--->:%" PRId64 "\n", GetAudioPts(&inctime, pFrame->nPts));
		break;

	default:
		break;
	}
}
static void writeFrames(TsParsedFrame pFrames[2])
{
	if (pFrames == NULL)
		return;
	for (int i = 0; i < 2; i++)
	{
		if (pFrames[i].pData == NULL)
			continue;
		writeFrame(&pFrames[i]);
	}
}

static int parsets(int argc, char **argv)
{
	int fd, bytes_read;
	uint8_t packet_buffer[TS_PACKET_SIZE];
	int n_packets = 0, n_signlePackets = 0;

	MpegTs tsParser;
	ts_init(&tsParser);

	InitLinkIncTime(&inctime);
    
	int inputIdx = 1;
	for (int i = 2; i < argc;){
		if (memcmp(argv[i], "-af", 3) == 0) {
			arg.pAudioOut = argv[++i];
			inputIdx = i + 1;
		}
		else if (memcmp(argv[i], "-vf", 3) == 0) {
			arg.pVideoOut = argv[++i];
			inputIdx = i + 1;
		} else {
			i++;
		}
	}

	for (int i = inputIdx; i < argc; i++){
		fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			printf("Error opening the stream\nSyntax: tsunpacket FileToParse.ts\n");
			return -1;
		}

		// Parse file while we can read full TS packets
		while (1) {
			bytes_read = read(fd, packet_buffer, TS_PACKET_SIZE);

			if (bytes_read < TS_PACKET_SIZE) {
				printf("End of file!\n");
				break;
			}
			TsParsedFrame frames[2];
			ts_parse_buffer(&tsParser, packet_buffer, bytes_read, frames);
			n_packets++;
			n_signlePackets++;
			writeFrames(frames);
		}
		// Freeing resources
		close(fd);

		printf("Number of packets found: %d %d\n", n_packets, n_signlePackets);
		n_signlePackets = 0;
		TsParsedFrame fFrame;
		if (ts_flush(&tsParser, &fFrame) > 0) {
			printf("flush one frame\n");
			writeFrames(&fFrame);
		}
	}

	if (arg.pAFile) {
		fclose(arg.pAFile);
	}
	if (arg.pVFile) {
		fclose(arg.pVFile);
	}

	ts_clean(&tsParser);

	return 0;
}

static void parseflv(int argc, char **argv) {
    AVD::Flv flv;
    std::string fname(argv[2]);
    auto fileReader = AVD::Flv::FileReader::NewFileReader(fname);
    if (!fileReader->IsOk()) {
        fprintf(stderr, "open %s fail\n", argv[2]);
        return;
    }
    int ret = 0;
    if ((ret = flv.Parse(fileReader)) == AVD::Flv::OK) {
        flv.Print();
    } else {
        printf("flv parse error:%d\n", ret);
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
        return -1;
    } else if (memcmp(argv[1], "-h", 2) == 0){
        usage(argc, argv);
        return 0;
    }
    
    if (memcmp(argv[1], "flv", 3) == 0) {
        parseflv(argc, argv);
    } else if (memcmp(argv[1], "ts", 2) == 0) {
        parsets(argc, argv);
    }

}
