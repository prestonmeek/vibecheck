MUSIC="/Users/testing/Desktop/edd code/microsd backup"
SAMPLERATE=32000
BITRATE=196k

# max=find "${MUSIC}/." -maxdepth 0 -type d | wc -l

cd "${MUSIC}"
max=$(ls | wc -l)

for i in `seq 1 $max`
do
    echo $i
    find "${MUSIC}/0${i}" -name "*.mp3" -execdir echo "{}" \; -exec mv "{}" "{}.mp3" \; -exec ffmpeg -y -loglevel "error" -i "{}.mp3" -acodec libmp3lame -ar ${SAMPLERATE} -b:a ${BITRATE} "{}" \; -exec rm "{}.mp3" \;
done

