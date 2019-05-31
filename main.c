#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "bitreader.h"

#include "parse.h"



int main(int argc, char **argv) {
	int fd, bytes_read;
	uint8_t packet_buffer[TS_PACKET_SIZE];
	int n_packets = 0;

	MpegTs tsParser;
	memset(&tsParser, 0, sizeof(tsParser));
	if (argc < 2) {
		printf("usage as:%s tsfilename", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		printf("Error opening the stream\nSyntax: tsunpacket FileToParse.ts\n");
		return -1;
	}

	// Parse file while we can read full TS packets
	while(1)
	{
		bytes_read = read(fd, packet_buffer, TS_PACKET_SIZE);

		if(bytes_read < TS_PACKET_SIZE) {
			printf("End of file!\n");
			break;
		}

		ts_parse_buffer(&tsParser, packet_buffer, bytes_read);
		n_packets++;
	}

	printf("Number of packets found: %d\n", n_packets);

	// Freeing resources
	close(fd);

	return 0;

}
