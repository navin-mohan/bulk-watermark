# Simple Bulk Watermarking tool
Batch watermarking tool based on Imagemagick library with multithreading support.

---

## Building
Make sure that you've imagemagick and gcc installed.
```bash
# installing Imagemagick on Ubuntu
sudo apt-get install imagemagick graphicsmagick-libmagick-dev-compat

# build
make

```

## Usage

```bash
# watermarking a list of images from  a file
./watermark < list.txt

# or use find to generate the list for you
# assuming the directory foo contains the images to be watermaked in jpg format
find /path/to/foo -type f -name "*.jpg" | ./watermark
```