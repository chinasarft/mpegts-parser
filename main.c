#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "tsparser.h"

typedef struct {
	const char *pVideoOut;
	FILE* pVFile;
	const char *pAudioOut;
	FILE* pAFile;
}CmdArg;
CmdArg arg;

static void usage(int argc, char **argv) {
	printf("usage as:%s tsfilename [options]", argv[0]);
	printf("options are:");
	printf("\t[-af afileout] audio stream out\n");
	printf("\t[-vf vfileout] video stream out\n");
}

static void writeFrame(TsParsedFrame *pFrame) {
	switch (pFrame->stype)
		{
		case TsTypeH264:
		case TsTypeH265:
			if (arg.pVFile == NULL) {
				arg.pVFile = fopen(arg.pVideoOut, "w");
			}
			if (arg.pVFile) {
				fwrite(pFrame->pData, 1, pFrame->nDataLen, arg.pVFile);
			}
			break;

		case TsTypePrivate:
		case TsTypeAAC:
			if (arg.pAFile == NULL) {
				arg.pAFile = fopen(arg.pAudioOut, "w");
			}
			if (arg.pAFile) {
				fwrite(pFrame->pData, 1, pFrame->nDataLen, arg.pAFile);
			}
			break;

			
		default:
			break;
		}
}
static void writeFrames(TsParsedFrame pFrames[2]) {
	if (pFrames == NULL)
		return;
	for(int i = 0; i < 2; i++) {
		if (pFrames[i].pData == NULL)
			continue;
		writeFrame(&pFrames[i]);
	}
}

int main(int argc, char **argv) {
	int fd, bytes_read;
	uint8_t packet_buffer[TS_PACKET_SIZE];
	int n_packets = 0;

	MpegTs tsParser;
	ts_init(&tsParser);

	if (argc < 2) {
		usage(argc, argv);
		return -1;
	} else if(memcmp(argv[1], "-h", 2) == 0) {
		usage(argc, argv);
		return 0;
	}

	fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		printf("Error opening the stream\nSyntax: tsunpacket FileToParse.ts\n");
		return -1;
	}
	for(int i = 2; i < argc;) {
		if(memcmp(argv[i], "-af", 3) == 0) {
			arg.pAudioOut = argv[++i];
		} else if(memcmp(argv[i], "-vf", 3) == 0) {
			arg.pVideoOut = argv[++i];
		} else {
			i++;
		}
	}

	// Parse file while we can read full TS packets
	while(1)
	{
		bytes_read = read(fd, packet_buffer, TS_PACKET_SIZE);

		if(bytes_read < TS_PACKET_SIZE) {
			printf("End of file!\n");
			break;
		}
		TsParsedFrame frames[2];
		ts_parse_buffer(&tsParser, packet_buffer, bytes_read, frames);
		n_packets++;
		writeFrames(frames);
	}

	printf("Number of packets found: %d\n", n_packets);

	TsParsedFrame fFrame;
	if (ts_flush(&tsParser, &fFrame) > 0) {
		printf("flush one frame\n");
		writeFrames(&fFrame);
	}

	// Freeing resources
	close(fd);

	if (arg.pAFile) {
		fclose(arg.pAFile);
	}
	if (arg.pVFile) {
		fclose(arg.pVFile);
	}

	ts_clean(&tsParser);

	return 0;

}
