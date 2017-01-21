//
// Copyright (c) 2017. See AUTHORS file.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef PIXL_IO_H
#define PIXL_IO_H

#include "image.h"
#include "utils.h"

namespace pixl {

    // Image Reader interface.
    class ImageReader {
    public:
        // Decodes an image and returns a pointer to that image.
        // The caller is responsible for deleting the image.
        virtual Image* read(const char* path) = 0;
    };


    // Image Writer interface.
    class ImageWriter {
    public:
        // Encodes an image and writes the file to the specified path.
        // The file extension of the path parameter determines wich image encoder
        // should be used.
        virtual void write(const char* path, Image* image) = 0;
    };


    // Simple image reader.
    //
    // This reader uses the stb_image library to decode various image formats.
    // The libraries goal is ease of use, not performance and saftey. So this
    // reader should only be used in a development enviroment.
    class StbiReader : public ImageReader {
    public:
        Image* read(const char* path);
    };

    // Simple image writer.
    //
    // This writer uses the stb_image_write library to encode data to png, bmp, tga & hdr.
    // The libraries goal is ease of use, not performance and saftey. Images encoded with this
    // writer
    // will result in 20-50% bigger images.
    class StbiWriter : public ImageWriter {
    public:
        void write(const char* path, Image* image);
    };


    // libpng writer
    //
    // This reader uses the official libpng library to decode png images.
    class PngReader : public ImageReader {
    public:
        Image* read(const char* path);
    };

    // libpng writer
    //
    // This writer uses the official libpng library to encode png images.
    class PngWriter : public ImageWriter {
    public:
        void write(const char* path, Image* image);
    };

    // libjpegturbo writer
    //
    // This reader uses the official libpng library to decode png images.
    class JpegTurboReader : public ImageReader {
    public:
        Image* read(const char* path);
    };

    // libjpegturbo writer
    //
    // This writer uses the libjpegturbo library to encode png images.
    class JpegTurboWriter : public ImageWriter {
    public:
        void write(const char* path, Image* image);
    };


    // Convenience function for decoding an image.
    //
    // This function internally picks an appropriate image decoder.
    static Image* read(const char* path) {
        // png
        if (is_png(path)) {
            PngReader reader;
            return reader.read(path);
            // jpg
        } else if (is_jpg(path)) {
            JpegTurboReader reader;
            return reader.read(path);
        }

        // other
        StbiReader reader;
        return reader.read(path);
    }

    // Convenience function for encoding an image.
    //
    // This function internally picks an appropriate image encoder.
    // Currently that's only StbiWriter.
    // The file extension of the path parameter determines wich image encoder
    // should be used.
    static void write(const char* path, Image* image) {
        // png
        if (is_png(path)) {
            PngWriter writer;
            return writer.write(path, image);
        }

        // other
        StbiWriter writer;
        writer.write(path, image);
    }
}

#endif