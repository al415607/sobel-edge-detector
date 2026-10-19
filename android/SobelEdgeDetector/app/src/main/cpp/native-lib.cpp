#include <jni.h>
#include <android/bitmap.h>

#include <cstdint>
#include <vector>

#include "SobelCore.hpp"

namespace
{
    void throwJavaException(
        JNIEnv* env,
        const char* className,
        const char* message
    )
    {
        jclass exceptionClass =
            env->FindClass(className);

        if (exceptionClass != nullptr)
        {
            env->ThrowNew(
                exceptionClass,
                message
            );
        }
    }

    class BitmapPixelsLock
    {
    public:
        BitmapPixelsLock(
            JNIEnv* env,
            jobject bitmap
        )
            : env_(env),
              bitmap_(bitmap)
        {
        }

        ~BitmapPixelsLock()
        {
            AndroidBitmap_unlockPixels(
                env_,
                bitmap_
            );
        }

        BitmapPixelsLock(
            const BitmapPixelsLock&
        ) = delete;

        BitmapPixelsLock& operator=(
            const BitmapPixelsLock&
        ) = delete;

    private:
        JNIEnv* env_;
        jobject bitmap_;
    };
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_sobeledgedetector_MainActivity_applySobel(
        JNIEnv* env,
        jobject /* this */,
        jobject bitmap)
{
    if (bitmap == nullptr)
    {
        throwJavaException(
            env,
            "java/lang/IllegalArgumentException",
            "El bitmap no puede ser null"
        );

        return;
    }

    AndroidBitmapInfo info;

    if (AndroidBitmap_getInfo(
            env,
            bitmap,
            &info
        ) != ANDROID_BITMAP_RESULT_SUCCESS)
    {
        throwJavaException(
            env,
            "java/lang/RuntimeException",
            "No se pudo obtener la informacion del bitmap"
        );

        return;
    }

    // Se trabaja con Bitmap.Config.ARGB_8888,
    // que en memoria nativa se maneja como RGBA de 4 bytes por pixel
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        throwJavaException(
            env,
            "java/lang/IllegalArgumentException",
            "El bitmap debe tener formato RGBA_8888"
        );

        return;
    }

    if (
        info.width < SobelCore::KERNEL_SIZE ||
        info.height < SobelCore::KERNEL_SIZE
    )
    {
        throwJavaException(
            env,
            "java/lang/IllegalArgumentException",
            "La imagen es demasiado pequena para el kernel Sobel"
        );

        return;
    }

    void* pixels = nullptr;

    if (AndroidBitmap_lockPixels(
            env,
            bitmap,
            &pixels
        ) != ANDROID_BITMAP_RESULT_SUCCESS)
    {
        throwJavaException(
            env,
            "java/lang/RuntimeException",
            "No se pudo acceder a los pixeles del bitmap"
        );

        return;
    }

    // A partir de aqui los pixeles se desbloquean automaticamente
    // al salir de la funcion, incluso si se produce una excepcion
    BitmapPixelsLock pixelLock(
        env,
        bitmap
    );

    const int width =
        static_cast<int>(info.width);

    const int height =
        static_cast<int>(info.height);

    auto* pixelData =
        static_cast<std::uint8_t*>(pixels);

    // Se guarda una copia en escala de grises para que el algoritmo
    // pueda leer los vecinos sin modificar los datos originales
    std::vector<std::uint8_t> gray(
        width * height
    );

    for (int y = 0; y < height; ++y)
    {
        std::uint8_t* row =
            pixelData + y * info.stride;

        for (int x = 0; x < width; ++x)
        {
            std::uint8_t* pixel =
                row + x * 4;

            const std::uint8_t red =
                pixel[0];

            const std::uint8_t green =
                pixel[1];

            const std::uint8_t blue =
                pixel[2];

            gray[y * width + x] =
                static_cast<std::uint8_t>(
                    0.299f * red +
                    0.587f * green +
                    0.114f * blue
                );
        }
    }

    // La salida se inicializa a cero para que los bordes exteriores
    // queden negros
    std::vector<std::uint8_t> edges(
        width * height,
        0
    );

    for (
        int y = SobelCore::KERNEL_RADIUS;
        y < height - SobelCore::KERNEL_RADIUS;
        ++y
    )
    {
        const std::uint8_t* previousRow =
            gray.data() + (y - 1) * width;

        const std::uint8_t* currentRow =
            gray.data() + y * width;

        const std::uint8_t* nextRow =
            gray.data() + (y + 1) * width;

        std::uint8_t* outputRow =
            edges.data() + y * width;

        SobelCore::processRow(
            previousRow,
            currentRow,
            nextRow,
            outputRow,
            width
        );
    }

    // Se copia el resultado Sobel al Bitmap RGBA
    for (int y = 0; y < height; ++y)
    {
        std::uint8_t* row =
            pixelData + y * info.stride;

        for (int x = 0; x < width; ++x)
        {
            const std::uint8_t edge =
                edges[y * width + x];

            std::uint8_t* pixel =
                row + x * 4;

            pixel[0] = edge;
            pixel[1] = edge;
            pixel[2] = edge;
            pixel[3] = 255;
        }
    }
}