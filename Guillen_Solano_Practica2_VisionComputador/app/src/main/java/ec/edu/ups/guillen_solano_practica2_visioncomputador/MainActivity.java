package ec.edu.ups.guillen_solano_practica2_visioncomputador;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.io.IOException;

public class MainActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2 {

    private static final String TAG = "MainActivity";
    private CameraBridgeViewBase vistaCamara;

    // Controles de estado
    private int intensidadRuido = 0;
    private int tamanoKernel = 1;
    private Mat imagenCatedral;

    // Variable para calcular los FPS manualmente
    private long tiempoAnterior = 0;

    static {
        System.loadLibrary("practica2_visioncomputador");
    }

    // Puente JNI con los nuevos parámetros
    public native void procesarChromaKey(long direccionMat, long direccionFondo, int ruido, int kernel);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (OpenCVLoader.initDebug()) {
            Log.i(TAG, "OpenCV inicializado");
        } else {
            Toast.makeText(this, "Fallo al inicializar OpenCV", Toast.LENGTH_LONG).show();
        }

        vistaCamara = findViewById(R.id.vista_camara);
        vistaCamara.setCvCameraViewListener(this);

        if (checkSelfPermission(android.Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{android.Manifest.permission.CAMERA}, 101);
        } else {
            vistaCamara.setCameraPermissionGranted();
            vistaCamara.enableView();
        }

        // SeekBar para inyección de ruido
        SeekBar controlRuido = findViewById(R.id.control_ruido);
        controlRuido.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                intensidadRuido = progress;
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        // SeekBar para atenuación (Desenfoque)
        SeekBar controlFiltro = findViewById(R.id.control_filtro);
        controlFiltro.setMax(10);
        controlFiltro.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // Cálculo matemático para garantizar que el kernel sea siempre un número impar
                tamanoKernel = (progress * 2) + 1;
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 101 && grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            vistaCamara.setCameraPermissionGranted();
            vistaCamara.enableView();
        } else {
            Toast.makeText(this, "Permiso de cámara denegado", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        // Carga de textura en memoria cuando se inicializa el sensor
        try {
            imagenCatedral = Utils.loadResource(this, R.drawable.catedral_cuenca);
        } catch (IOException e) {
            e.printStackTrace();
            imagenCatedral = new Mat();
        }
    }

    @Override
    public void onCameraViewStopped() {
        if (imagenCatedral != null) {
            imagenCatedral.release();
        }
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        Mat frame = inputFrame.rgba();

        // 1. Procesamiento C++
        if (imagenCatedral != null && !imagenCatedral.empty()) {
            procesarChromaKey(frame.getNativeObjAddr(), imagenCatedral.getNativeObjAddr(), intensidadRuido, tamanoKernel);
        }

        // 2. Cálculo y Dibujo Manual de FPS (Infalible)
        long tiempoActual = System.currentTimeMillis();
        if (tiempoAnterior != 0) {
            long diferencia = tiempoActual - tiempoAnterior;
            if (diferencia > 0) {
                // Cálculo: 1000 ms divididos por el tiempo que tardó el frame
                int fps = (int) (1000 / diferencia);
                String textoFps = "FPS: " + fps;

                // Coordenadas: Esquina superior derecha
                org.opencv.core.Point posicion = new org.opencv.core.Point(frame.cols() - 350, 100);

                // Color: Rojo
                org.opencv.core.Scalar color = new org.opencv.core.Scalar(255, 0, 0, 255);

                // Imprimimos el texto directamente sobre la matriz
                org.opencv.imgproc.Imgproc.putText(frame, textoFps, posicion, org.opencv.imgproc.Imgproc.FONT_HERSHEY_SIMPLEX, 2.0, color, 5);
            }
        }
        tiempoAnterior = tiempoActual;

        return frame;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (vistaCamara != null) {
            vistaCamara.disableView();
        }
    }
}