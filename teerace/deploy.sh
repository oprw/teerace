# first run
# apt install build-essential cmake git libfreetype6-dev libsdl2-dev libpnglite-dev libwavpack-dev python3 unzip
# cd /root/teerace && wget https://github.com/matricks/bam/archive/v0.5.1.tar.gz && tar xvzf v0.5.1.tar.gz bam-0.5.1/ && cd bam-0.5.1/ && ./make_unix.sh && cd .. && ./bam-0.5.1/bam config


./bam-0.5.1/bam -f conf=release server arch=x86_64 && cp build/x86_64/release/teeworlds_srv /root/server/
