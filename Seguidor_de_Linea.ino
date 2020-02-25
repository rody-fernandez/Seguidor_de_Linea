/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                             //
//                      CALIBRAR EL ROBOT DETRO DE LA LÍNEA NEGRA (ENCENDER DENTRO DE LA LÍNEA)                                //
//                                                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Incluimos la librería de los motores
#include <LEANTEC_ControlMotor.h>
// Configuramos los pines que vamos a usar
ControlMotor control(2, 3, 7, 4, 5, 6); // MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo

#define PIN_LDR1 2 // Define el pin analog de LDR 1
#define PIN_LDR2 3 // Define el pin analog de LDR 2
#define PIN_FL1 0  // Define el pin analog de IR  1 (Este sensor no se usa en este programa)
#define PIN_FL2 1  // Define el pin analog de IR  2 (Este sensor no se usa en este programa)

#define P_MUERTO 10 // Valor que tendrá que superar el cambio de luz para que gire el robot +/- P_MUERTO

#define GIRO_LINEA 50  // Máximo giró que realizará el sensor (entre 0 y 100), 100 sería parado girando sobre su eje
#define GIRO_FUERA 100 // Giro que hace el robot cuando se sale de la línea

#define VELOCIDAD_NORMAL 255    // Velocidad a la que se desplazará el robot dentro de la línea
#define VELOCIDAD_BAJA 200      // Velocidad a la que se desplaza el robot cuando sale un sensor fuera de la línea
#define VELOCIDAD_BAJA_GIRO 150 // Velocidad a la que gira el robot para localizar la línea cuando se ha salido los dos sensores

int OFFSET_LDR1 = 0; // (Este sensor no se usa en este programa)
int OFFSET_LDR2 = 0; // (Este sensor no se usa en este programa)
int OFFSET_FL1 = 0;  // Ajusto el punto central del sensor para que los dos estén iguales (en este caso, se ajusta solo al iniciar el robot
int OFFSET_FL2 = 0;  // Ajusto el punto central del sensor para que los dos estén iguales (en este caso, se ajusta solo al iniciar el robot

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Estos valores de abajo no es necesario cambiarlos
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int sensores[4];            // Array donde se guardarán los valores
int valor_neutro = 0;       // Este valor se entiende que es el valor en negro, por lo que se debe calibrar en negro.
int velocidad = 255;        // Velocidad a la que se desplaza el robot
int direccion_anterior = 0; // Recuerda que sensor se salío de la línea para girar en el sentido correcto y localizar la línea de nuevo

#define LDR1 0 // Define la posición en el ARRAY donde se guardará el valor de la LDR 1 (Este sensor no se usa en este programa)
#define LDR2 1 // Define la posición en el ARRAY donde se guardará el valor de la LDR 2 (Este sensor no se usa en este programa)
#define FL1 2  // Define la posición en el ARRAY donde se guardará el valor de la IR  1
#define FL2 3  // Define la posición en el ARRAY donde se guardará el valor de la IR  2
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void leer_sensores();
int calcula_diferencia();
void mostrar_valores();

//----------------------------- Configuramos Ardunio ---------------------------------------
//------------------------------------------------------------------------------------------
void setup(void)
{
  // Iniciamos el puerto de comunicación
  Serial.begin(9600);

  //********************************* Calibramos los sensores ****************************************************//
  leer_sensores(); // Leemos sensores con los OFFSET a 0
  if (sensores[FL1] > sensores[FL2])
  {                                             // Comprobamos si FL1 es mayor que FL2
    OFFSET_FL1 = 0;                             // Como FL1 es mayor, sumamos a FL2
    OFFSET_FL2 = sensores[FL1] - sensores[FL2]; // Sumamos a FL2 la diferencia para que esten igualadas las dos
  }
  else
  {                                             // Si hemos llegado aquí, es porque LDR2 o igual que FL1
    OFFSET_FL2 = 0;                             // Como FL2 es mayor, sumamos a FL1
    OFFSET_FL1 = sensores[FL2] - sensores[FL1]; // Sumamos a FL1 la diferencia para que esten igualadas las dos
  }
  //**************************************************************************************************************//
  valor_neutro = sensores[FL1]; // Almacenamos unos de los sensores (Ambos son iguales en este punto) como valor_neutro.
                                // Si ambos sensores sobrepasan este valor + P_MUERTO, se entiende que el robot esta fuera de línea por completo
}
//--------------------------- Arrancamos el programa ---------------------------------------
//------------------------------------------------------------------------------------------
void loop(void)
{
  leer_sensores(); // Leemos los sensores con los OFFSET ya calculados
  //mostrar_valores();  // Mostramos los valores por si tenemos que comprobar algún dato
  control.Motor(velocidad, calcula_diferencia()); // Movemos el robot a la velocidad seleccionada y el giro calculado
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Mostramos los valores por el monitor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mostrar_valores()
{
  Serial.print(" OFF_1 = "); // Imprimimos el OFFSET del FL1
  Serial.print(OFFSET_FL1);
  Serial.print(" OFF_2 = "); // Imprimimos el OFFSET del FL2
  Serial.print(OFFSET_FL2);
  Serial.print(" FL_1 = "); // Imprimimos el valor del FL1
  Serial.print(sensores[FL1]);
  Serial.print(" FL_2 = "); // Imprimimos el valor del FL2
  Serial.print(sensores[FL2]);
  Serial.print(" DIF = "); // Imprimimos la diferencia entre el sensor 1 y el sensor 2
  Serial.println(calcula_diferencia());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Leemos los sensores y ajustamos los OFFSET
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void leer_sensores()
{
  sensores[LDR1] = analogRead(PIN_LDR1) + OFFSET_LDR1; // Leemos y guardamos el valor del sensor con el OFFSET incluido
  if (sensores[LDR1] < 0)
  {
    sensores[LDR1] = 0;
  } // Comprobamos que con el OFFSET no sea menor de 0
  else if (sensores[LDR1] > 1023)
  {
    sensores[LDR1] = 1023;
  } // Comprobamos que con el OFFSET no sea mayor de 0

  sensores[LDR2] = analogRead(PIN_LDR2) + OFFSET_LDR2; // Leemos y guardamos el valor del sensor con el OFFSET incluido
  if (sensores[LDR2] < 0)
  {
    sensores[LDR2] = 0;
  } // Comprobamos que con el OFFSET no sea menor de 0
  else if (sensores[LDR2] > 1023)
  {
    sensores[LDR2] = 1023;
  } // Comprobamos que con el OFFSET no sea mayor de 0

  sensores[FL1] = analogRead(PIN_FL1) + OFFSET_FL1; // Leemos y guardamos el valor del sensor con el OFFSET incluido
  if (sensores[FL1] < 0)
  {
    sensores[FL1] = 0;
  } // Comprobamos que con el OFFSET no sea menor de 0
  else if (sensores[FL1] > 1023)
  {
    sensores[FL1] = 1023;
  } // Comprobamos que con el OFFSET no sea mayor de 0

  sensores[FL2] = analogRead(PIN_FL2) + OFFSET_FL2; // Leemos y guardamos el valor del sensor con el OFFSET incluido
  if (sensores[FL2] < 0)
  {
    sensores[FL2] = 0;
  } // Comprobamos que con el OFFSET no sea menor de 0
  else if (sensores[FL2] > 1023)
  {
    sensores[FL2] = 1023;
  } // Comprobamos que con el OFFSET no sea mayor de 0
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Calculamos la diferencia que hay de un sensor a otro
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int calcula_diferencia()
{
  int valor; // En esta variable almacenamos el valor positivo o negativo del resultado de la función

  if ((sensores[FL1] > valor_neutro + P_MUERTO) && (sensores[FL2] > valor_neutro + P_MUERTO))
  {
    if (direccion_anterior == 0)
    {
      valor = 100;
    }
    else
    {
      valor = -100;
    }
    velocidad = VELOCIDAD_BAJA_GIRO;
  }
  else
  {
    valor = sensores[FL2] - sensores[FL1]; // Restamos el valor de la FL 2 al valor de la FL 1, la dirección se calcula viendo si el valor es positivo o negativo.

    if ((valor > P_MUERTO) || (valor < -P_MUERTO))
    { // Si el valor cambia por encima de P_MUERTO el robot cambiará de posición según la diferencia de un sensor a otros
      velocidad = VELOCIDAD_BAJA;
      if (valor > 0)
      {
        valor = GIRO_LINEA;
        direccion_anterior = 0;
      } // Giramos a la derecha para entrar de nuevo en la línea
      if (valor < 0)
      {
        valor = -GIRO_LINEA;
        direccion_anterior = 1;
      } // Giramos a la izquierda para entrar de nuevo en la línea
    }
    else
    {                               // Si el cambio es menor al punto muerto, el robot no gira
      valor = 0;                    // Dejamos que el robot sigua recto
      velocidad = VELOCIDAD_NORMAL; // El robot avanza a esta velocidad dentro de la línea
    }
  }

  return valor; // Devolvemos el valor
}
Obra publicada con Licencia Creative Commons Reconocimiento Compartir igual 4.0

« Anterior Siguiente » Aníbal de la Torre - 2017
