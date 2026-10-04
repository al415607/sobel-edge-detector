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

        binding.applySobelButton.setOnClickListener(view -> {
            if (selectedBitmap == null) {
                return;
            }

            // Se crea una copia modificable para procesarla desde C++
            Bitmap processedBitmap = selectedBitmap.copy(
                    Bitmap.Config.ARGB_8888,
                    true
            );

            applySobel(processedBitmap);

            binding.imageView.setImageBitmap(processedBitmap);
        });
    }

    private void loadImage(Uri uri) {
        try (InputStream inputStream =
                     getContentResolver().openInputStream(uri)) {

            selectedBitmap = BitmapFactory.decodeStream(inputStream);

            if (selectedBitmap == null) {
                Toast.makeText(
                        this,
                        "No se pudo cargar la imagen",
                        Toast.LENGTH_SHORT
                ).show();
                return;
            }

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