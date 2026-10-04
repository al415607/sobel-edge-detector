#include <jni.h>
#include <android/bitmap.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

extern "C"
JNIEXPORT void JNICALL
Java_com_example_sobeledgedetector_MainActivity_applySobel(
        JNIEnv* env,
        jobject /* this */,
        jobject bitmap)
{
    AndroidBitmapInfo info;

    if (AndroidBitmap_getInfo(env, bitmap, &info)
        != ANDROID_BITMAP_RESULT_SUCCESS)
    {
        return;
    }

    // Se trabaja con Bitmap.Config.ARGB_8888,
    // que en memoria nativa se maneja como RGBA de 4 bytes por pixel
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        return;
    }

    void* pixels = nullptr;

    if (AndroidBitmap_lockPixels(env, bitmap, &pixels)
        != ANDROID_BITMAP_RESULT_SUCCESS)
    {
        return;
    }

    const int width = static_cast<int>(info.width);
    const int height = static_cast<int>(info.height);

    // Se guarda primero una copia de la imagen en escala de grises
    // Es necesario porque no se puede sobrescribir los pixeles mientras
    // todavia se necesitan para calcular sus vecinos
    std::vector<unsigned char> gray(width * height);

    auto* pixelData = static_cast<unsigned char*>(pixels);

    for (int y = 0; y < height; ++y)
    {
        unsigned char* row = pixelData + y * info.stride;

        for (int x = 0; x < width; ++x)
        {
            unsigned char* pixel = row + x * 4;

            const unsigned char red = pixel[0];
            const unsigned char green = pixel[1];
            const unsigned char blue = pixel[2];

            // Conversion de RGB a intensidad
            gray[y * width + x] =
                    static_cast<unsigned char>(
                            0.299 * red +
                            0.587 * green +
                            0.114 * blue
                    );
        }
    }

    // Los pixeles exteriores se dejan negros porque el kernel Sobel
    // necesita los vecinos alrededor del pixel que se procesa
    for (int x = 0; x < width; ++x)
    {
        unsigned char* topPixel =
                pixelData + x * 4;

        unsigned char* bottomPixel =
                pixelData + (height - 1) * info.stride + x * 4;

        topPixel[0] = topPixel[1] = topPixel[2] = 0;
        bottomPixel[0] = bottomPixel[1] = bottomPixel[2] = 0;
    }

    for (int y = 0; y < height; ++y)
    {
        unsigned char* row = pixelData + y * info.stride;

        unsigned char* leftPixel = row;
        unsigned char* rightPixel = row + (width - 1) * 4;

        leftPixel[0] = leftPixel[1] = leftPixel[2] = 0;
        rightPixel[0] = rightPixel[1] = rightPixel[2] = 0;
    }

    for (int y = 1; y < height - 1; ++y)
    {
        unsigned char* outputRow =
                pixelData + y * info.stride;

        for (int x = 1; x < width - 1; ++x)
        {
            const int gx =
                    -gray[(y - 1) * width + (x - 1)]
                    + gray[(y - 1) * width + (x + 1)]
                    - 2 * gray[y * width + (x - 1)]
                    + 2 * gray[y * width + (x + 1)]
                    - gray[(y + 1) * width + (x - 1)]
                    + gray[(y + 1) * width + (x + 1)];

            const int gy =
                    -gray[(y - 1) * width + (x - 1)]
                    - 2 * gray[(y - 1) * width + x]
                    - gray[(y - 1) * width + (x + 1)]
                    + gray[(y + 1) * width + (x - 1)]
                    + 2 * gray[(y + 1) * width + x]
                    + gray[(y + 1) * width + (x + 1)];

            const double magnitude =
                    std::sqrt(
                            static_cast<double>(
                                    gx * gx + gy * gy
                            )
                    );

            const unsigned char edge =
                    static_cast<unsigned char>(
                            std::min(magnitude, 255.0)
                    );

            unsigned char* outputPixel =
                    outputRow + x * 4;

            // La imagen de bordes se representa en escala de grises,
            // poniendo el mismo valor en R, G y B
            outputPixel[0] = edge;
            outputPixel[1] = edge;
            outputPixel[2] = edge;
            outputPixel[3] = 255;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
}