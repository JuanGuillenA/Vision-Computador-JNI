#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace cv;
using namespace std;

extern "C"
JNIEXPORT void JNICALL
Java_ec_edu_ups_guillen_1solano_1practica2_1visioncomputador_MainActivity_procesarChromaKey(
        JNIEnv *env, jobject thiz, jlong direccionMat, jlong direccionFondo, jint ruidoIntensidad, jint kernelBlur) {

    // 1. Recuperamos el frame de la cámara y la foto de la catedral
    Mat& frame = *(Mat*)direccionMat;
    Mat& fondoOriginal = *(Mat*)direccionFondo;

    // 2. Convertir cámara a HSV
    Mat frameRGB, hsv;
    cvtColor(frame, frameRGB, COLOR_RGBA2RGB);
    cvtColor(frameRGB, hsv, COLOR_RGB2HSV);

    // 3. Segmentación Chroma Key (Modo Estricto Antirreflejos)
    // H (95-135): Rango del azul.
    // S (120): Exigimos un color muy fuerte y puro (ignora reflejos débiles).
    // V (90): Exigimos luz brillante (protege el cabello que es oscuro).
    Scalar lower_blue(95, 120, 90);
    Scalar upper_blue(135, 255, 255);
    Mat mask, maskInv;

    // Identifica qué partes de la imagen son azules
    inRange(hsv, lower_blue, upper_blue, mask);

    // Invertimos la máscara para aislar al usuario
    bitwise_not(mask, maskInv);

    // Refinamiento de máscara: Eliminar pequeños ruidos y rellenar huecos
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    // Invertimos la máscara para aislar al usuario
    bitwise_not(mask, maskInv);

    // 4. Procesar el fondo de la Catedral
    Mat fondoResized;
    // La foto debe medir exactamente lo mismo que el video de la cámara
    resize(fondoOriginal, fondoResized, frameRGB.size());

    // CORRECCIÓN DE COLOR: OpenCV lee la foto en BGR, pero la cámara está en RGB.
    // Aquí invertimos los canales Rojo y Azul para arreglar el cielo naranja.
    if (fondoResized.channels() == 4) {
        cvtColor(fondoResized, fondoResized, COLOR_BGRA2RGB);
    } else {
        cvtColor(fondoResized, fondoResized, COLOR_BGR2RGB);
    }

    // 5. Separar el usuario (foreground) y el fondo de la catedral (background)
    Mat foreground, background;
    // Obtenemos al usuario donde la máscara invertida es blanca
    bitwise_and(frameRGB, frameRGB, foreground, maskInv);
    // Obtenemos la catedral SOLO donde la máscara original es blanca (el fondo verde)
    bitwise_and(fondoResized, fondoResized, background, mask);

    // 6. Inyectar Ruido Gaussiano al usuario
    if (ruidoIntensidad > 0) {
        Mat noise = Mat::zeros(foreground.size(), foreground.type());
        randn(noise, 0, ruidoIntensidad); // Generación de distribución estocástica
        add(foreground, noise, foreground, maskInv); // Sumamos el ruido solo al usuario
    }

    // 7. Aplicar Filtro espacial (Filtro de la Mediana)
    if (kernelBlur >= 3) {
        medianBlur(foreground, foreground, kernelBlur);
    }

    // 8. Combinar finalmente al usuario con la Catedral
    Mat resultado;
    add(foreground, background, resultado);

    // 9. Devolver la imagen procesada hacia Android
    cvtColor(resultado, frame, COLOR_RGB2RGBA);
}

/* =========================================================================
   PARTE 1-A: MANIPULACIÓN DE CANALES Y ALPHA BLENDING (EFECTO STRUSS)
   INSTRUCCIONES: Para evaluar la Parte 1-A, descomente este bloque
   y comente el bloque de la función de la Parte 1-B.
   ========================================================================= */
/*
extern "C"
JNIEXPORT void JNICALL
Java_ec_edu_ups_guillen_1solano_1practica2_1visioncomputador_MainActivity_procesarAlphaBlending(
        JNIEnv *env, jobject thiz, jlong direccionFrame, jlong direccionFondo, jint alphaProgress) {

    // 1. Obtener las matrices desde Java
    Mat& frame = *(Mat*)direccionFrame;
    Mat& fondo = *(Mat*)direccionFondo;

    // 2. Convertir el frame de la cámara de RGBA a RGB
    Mat frameRGB;
    cvtColor(frame, frameRGB, COLOR_RGBA2RGB);

    // 3. Redimensionar y corregir color del fondo (BGR a RGB)
    Mat fondoResized;
    resize(fondo, fondoResized, frameRGB.size());
    if (fondoResized.channels() == 4) {
        cvtColor(fondoResized, fondoResized, COLOR_BGRA2RGB);
    } else {
        cvtColor(fondoResized, fondoResized, COLOR_BGR2RGB);
    }

    // =========================================================
    // CRITERIO 2: SEPARACIÓN DE CANALES (EFECTO STRUSS)
    // =========================================================
    vector<Mat> canales;
    split(frameRGB, canales);

    // Ejemplo de manipulación: Anular el canal Verde
    canales[1] = Mat::zeros(canales[1].size(), canales[1].type());

    Mat frameModificado;
    merge(canales, frameModificado);

    // =========================================================
    // CRITERIO 2: ALPHA BLENDING (TRANSICIÓN)
    // =========================================================
    double alpha = alphaProgress / 100.0;
    double beta = 1.0 - alpha;

    Mat blendingResult;
    addWeighted(frameModificado, alpha, fondoResized, beta, 0.0, blendingResult);

    // Devolver el resultado
    blendingResult.copyTo(frame);
}
*/