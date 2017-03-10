#!/bin/bash

input=$1
segmentTime=$2 
videoSequenceName=videoSequence
hlsConfigFileName=list.m3u8
clearFolder=clear_video
version="0.1.170309"
author="Ricardo Costa"

#Check if Arguments are set correctly
if [ -z "$input" ] || [ -z "$segmentTime" ] 
then
    echo "gen_hls.sh - Script for creating hls format. Version:${version} Author:${author}"
    echo "Syntax: gen_hls.sh <VIDEO_FILE_NAME [*.mp4]> <CHUNK_TIME_INTERVAL_IN_SECS>"
    echo ""
    echo "Ex: ./gen_hls.sh /videos/video_test.mp4 5"
    exit 1
fi

#Create clear folder
mkdir -p ${clearFolder}

echo -e "\e[1;33m=> All files will be created in ./clear_video folder!\e[0m"
echo -e "\e[1;33m=> HLS Manifest file name : list.m3u8\e[0m"
echo -e "\e[1;33m=> HLS Chunks file name: videoSequence[0-N].ts\e[0m"

#Create clear video segments
ffmpeg -y -i $input -hls_time ${segmentTime} -hls_playlist_type vod -hls_segment_filename "${clearFolder}/${videoSequenceName}%d.ts" ${clearFolder}/${hlsConfigFileName}

