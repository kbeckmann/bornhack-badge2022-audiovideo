#!/bin/bash
set -ex

rm -rf ./raw

mkdir raw

# Center crop
# width = 1920
# height = 1080
# out_w = (128/160) * height
# out_h = height
# x = width/2 - out_w/2

# yt-dlp https://www.youtube.com/watch?v=dQw4w9WgXcQ
# ffmpeg \
# 	-ss 00:00:00.3 -to 00:00:08.0 \
# 	-i rick.mkv \
# 	-filter_complex "crop=864:1080:528:0,scale=129x161" \
#     -vcodec rawvideo -f rawvideo -pix_fmt rgb565 \
#     raw/frames.rgb

# yt-dlp https://www.youtube.com/watch?v=d-eAoSNUy1E
# ffmpeg \
# 	-ss 00:00:00.3 -to 00:00:08.0 \
# 	-i badapple.mkv \
# 	-filter_complex "fps=25,crop=1536:2880:1152:0,scale=129x161" \
#     -vcodec rawvideo -f rawvideo -pix_fmt rgb565 \
#     raw/frames.rgb

# yt-dlp https://www.youtube.com/watch?v=n5n7CSGPzqw
# ffmpeg \
# 	-ss 00:00:00.0 -to 00:00:04.0 \
# 	-i baka.webm \
# 	-filter_complex "fps=25,crop=384:480:128:0,scale=129x161" \
#     -vcodec rawvideo -f rawvideo -pix_fmt rgb565 \
#     raw/frames.rgb

# yt-dlp https://www.youtube.com/watch?v=MTIwzKI44Es
# ffmpeg \
# 	-ss 00:00:00.0 -to 00:00:04.0 \
# 	-i leek.webm \
# 	-filter_complex "fps=25,crop=864:1080:288:0,scale=129x161" \
#     -vcodec rawvideo -f rawvideo -pix_fmt rgb565 \
#     raw/frames.rgb

# yt-dlp https://www.youtube.com/watch?v=kuNixp-wvWM
# 960x720
ffmpeg \
	-ss 00:00:00.0 -to 00:00:04.0 \
	-i fukkireta.webm \
	-filter_complex "fps=25,crop=576:720:192:0,scale=129x161" \
    -vcodec rawvideo -f rawvideo -pix_fmt rgb565 \
    raw/frames.rgb

dd if=raw/frames.rgb of=raw/frames_swapped.rgb conv=swab
python3 make_blob.py raw/frames_swapped.rgb 25 video_data.bin


ffmpeg \
	-i fukkireta.webm \
    raw/audio.wav

sox raw/audio.wav --bits 8 --channels 1 --encoding unsigned-integer --rate 24000 audio_data.raw


echo "Now run:"
echo "  picotool load -v -t bin video_data.bin -o 0x10010000"
echo "  picotool load -v -t bin audio_data.raw -o 0x10800000"
echo " "
