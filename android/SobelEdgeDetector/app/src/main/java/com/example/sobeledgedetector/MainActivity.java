package com.example.sobeledgedetector;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Toast;

import com.example.sobeledgedetector.databinding.ActivityMainBinding;

import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    // Carga de la libreria C++ de la aplicacion
    static {
        System.loadLibrary("sobeledgedetector");
    }

    private ActivityMainBinding binding;

    // Imagen seleccionada
    private Bitmap selectedBitmap;

    // Abre el selector de imagenes y recibe la imagen elegida
    private final ActivityResultLauncher<String> imagePicker =
            registerForActivityResult(
                    new ActivityResultContracts.GetContent(),
                    uri -> {
                        if (uri != null) {
                            loadImage(uri);
                        }
                    }
            );

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // No se puede aplicar Sobel hasta seleccionar una imagen
        binding.applySobelButton.setEnabled(false);

        binding.selectImageButton.setOnClickListener(view ->
                imagePicker.launch("image/*")
        );

        binding.applySobelButton.setOnClickListener(view ->
                processSelectedImage()
        );
    }

    private void processSelectedImage() {
        if (selectedBitmap == null) {
            Toast.makeText(
                    this,
                    "Primero selecciona una imagen",
                    Toast.LENGTH_SHORT
            ).show();

            return;
        }

        // Se evita iniciar varias operaciones al mismo tiempo
        binding.applySobelButton.setEnabled(false);

        try {
            // Se crea una copia modificable en el formato esperado por C++
            Bitmap processedBitmap = selectedBitmap.copy(
                    Bitmap.Config.ARGB_8888,
                    true
            );

            if (processedBitmap == null) {
                Toast.makeText(
                        this,
                        "No se pudo preparar la imagen",
                        Toast.LENGTH_SHORT
                ).show();

                return;
            }

            applySobel(processedBitmap);

            binding.imageView.setImageBitmap(processedBitmap);

        } catch (IllegalArgumentException exception) {
            Toast.makeText(
                    this,
                    "Imagen no valida: " + exception.getMessage(),
                    Toast.LENGTH_SHORT
            ).show();

        } catch (RuntimeException exception) {
            Toast.makeText(
                    this,
                    "Error al procesar la imagen: " + exception.getMessage(),
                    Toast.LENGTH_SHORT
            ).show();

        } finally {
            binding.applySobelButton.setEnabled(true);
        }
    }

    private void loadImage(Uri uri) {
        try (InputStream inputStream =
                     getContentResolver().openInputStream(uri)) {

            if (inputStream == null) {
                Toast.makeText(
                        this,
                        "No se pudo abrir la imagen",
                        Toast.LENGTH_SHORT
                ).show();

                return;
            }

            Bitmap bitmap =
                    BitmapFactory.decodeStream(inputStream);

            if (bitmap == null) {
                Toast.makeText(
                        this,
                        "No se pudo cargar la imagen",
                        Toast.LENGTH_SHORT
                ).show();

                return;
            }

            selectedBitmap = bitmap;

            binding.imageView.setImageBitmap(selectedBitmap);
            binding.applySobelButton.setEnabled(true);

        } catch (IOException exception) {
            Toast.makeText(
                    this,
                    "Error al leer la imagen",
                    Toast.LENGTH_SHORT
            ).show();
        }
    }

    public native void applySobel(Bitmap bitmap);
}