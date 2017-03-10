HLS for Encryption and Decryption - hls4encdec
================================================

Introduction
------------
hls4encdec is an application for encryption and decryption HLS chunks. 

The encryption and decryption are done by OpenSSL functions. 

It uses AES-128-CBC algorithm and it was written in pure C language. 

It reads a HLS Manifest file (*.m3u8) and encrypts/decrypts all chunks provided in it.

For decryption, it generates a Comma-Separated Values (CSV) with final decryption benchmark time.

Actually, the key is hardcoded in the source code.

License
-------
See LICENSE file for details.

Build the hls4encdec
--------------------
###Prerequisites:
- Ensure that you have the following required operating systems: 
  Ubuntu\* Desktop-14.04-LTS 64bits
- Use the following command to install the required tools to build hls4encdec: 
```
  $ sudo apt-get install openssl nasm
```
- Use the following command to install additional required tools for creating hls chunks and manifest file (gen_hls.sh): 
```
  $ sudo apt-get install ffmpeg
```

- Use the following command to install additional required tools for playing the hls streaming (vlc): 
```
  $ sudo apt-get install vlc
```

###Build the hls4encdec
The following steps describe how to build the hls4endec application.

- To build it, enter the following command: 

```
  $ make 
``` 

- To clean the files generated by previous `make` command, enter the following command: 
```
  $ make clean
```

The script file for generating hls chunks (gen_hls.sh) and hls4encdec application will be saved into ``bin`` folder.

Run the hls4encdec
--------------------
###Generating HLS chunks for VOD Type
To generate the HLS chunk for VOD Type, enter the following commands:
```
$ cd bin
```
```
$ chmod 775 gen_hls.sh
```
To run the script (ffmpeg), enter the following commands:

```
$ ./gen_hls.sh <PATH>/VIDEO_FILENAME.mp4 <CHUNK_TIME_INTERVAL_IN_SECONDS>
```
All files (videoSequence%%.ts and list.m3u8) will be saved into ``./clear_video`` folder. 

###Encrypt HLS Chunks
To encrypt HLS chunks, enter the following command into ``bin`` folder:
```
$ ./hls4encdec -e -i <CLEAR_VIDEO_PATH>/<HLS_MANIFEST_FILENAME>.m3u8
```

All encrypted files (videoSequence%%.ts and list.m3u8) will be saved into ``./encrypted_video`` folder. 


###Decrypt HLS Chunks
To decrypt HLS chunks, enter the following command into ``bin`` folder:
```
$ ./hls4encdec -d -i <ENCRYPTED_PATH>/<HLS_MANIFEST_FILENAME>.m3u8 -r <PATH>/<CSV_RESULT_FILENAME>.csv
```

All decrypted files (videoSequence%%.ts and list.m3u8) will be saved into ``./decrypted_video`` folder. 

The csv file will contain the chunk file name followed by decryption benchmark time.

CSV data:

```
Chunk Filename, Decrypt Process Time (Seconds)
encrypted_video/videoSequence0.ts, 0.002120
encrypted_video/videoSequence1.ts, 0.002045
encrypted_video/videoSequence2.ts, 0.001358
encrypted_video/videoSequence3.ts, 0.001955
encrypted_video/videoSequence4.ts, 0.001312
encrypted_video/videoSequence5.ts, 0.001951
encrypted_video/videoSequence6.ts, 0.001756
encrypted_video/videoSequence7.ts, 0.001882
encrypted_video/videoSequence8.ts, 0.001725
encrypted_video/videoSequence9.ts, 0.002019
```
Test Decrypted Video
-------------------------
For playing the decrypted video, enter the following command to run the vlc player:
```
$ vlc <DECRYPTED_PATH>/<HLS_MANIFEST_FILENAME>.m3u8
```

