/*
 * classify.ino
 *
 * EE16B Spring 2016
 * Emily Naviasky & Nathaniel Mailoa
 *
 * EE 16B Fall 2017
 * Andrew Blatner
 *
 */

#define MIC_INPUT                   P6_0

#define SIZE                        2752
#define SIZE_AFTER_FILTER           172
#define ADC_TIMER_MS                0.35

/*---------------------------*/
/*      CODE BLOCK PCA1      */
/*---------------------------*/

// Enveloping and K-means constants
#define SNIPPET_SIZE                100
#define PRELENGTH                   5
#define THRESHOLD                   0.6

#define KMEANS_THRESHOLD            0.045
#define LOUDNESS_THRESHOLD          950

/*---------------------------*/
/*      CODE BLOCK PCA2      */
/*---------------------------*/

float pca_vec1[100] = {-0.046285687348, -0.0448866994267, -0.0447258262842, -0.0444718000176, -0.0975033748264, -0.15933506406, -0.164168638958, -0.19290879541, -0.203248686339, -0.211242930659, -0.211485847697, -0.211277889634, -0.214082617155, -0.191490764583, -0.216880979732, -0.206878210296, -0.189083248486, -0.206904254904, -0.206540835371, -0.183658775122, -0.189452625916, -0.152246254287, -0.172525139436, -0.130802158557, -0.0968991023281, -0.0862779432884, -0.0513024149089, -0.0471995881233, -0.0238933984925, -0.00305482835846, 0.017955639593, 0.0348661629223, 0.0381941698973, 0.0485520836543, 0.0460024998843, 0.0529574412522, 0.0573750115901, 0.0482648534306, 0.0572894630523, 0.0447349017185, 0.0570514640131, 0.0535328244399, 0.053795996953, 0.0630253325041, 0.0577057269514, 0.0629034170428, 0.0667116921519, 0.0686640057196, 0.0866476835745, 0.0777672224187, 0.0869658863054, 0.0772838410909, 0.0853745826038, 0.084566310353, 0.0819622962975, 0.0838766558654, 0.0705344184403, 0.0706356006325, 0.06715480654, 0.0641218833568, 0.0635379611984, 0.0538702778754, 0.0482897651462, 0.0567773760169, 0.0492564267145, 0.0467750448284, 0.0475188536071, 0.0519378456942, 0.0549924761579, 0.0550272761826, 0.0497887518146, 0.0548957025456, 0.056581131585, 0.0574923416218, 0.0616172772638, 0.0635796851409, 0.0596624538629, 0.0652586802157, 0.0559194702137, 0.0666646545842, 0.0622568638082, 0.0633316907515, 0.0632454223559, 0.0614660054058, 0.0643661134583, 0.0668554420963, 0.0653228502654, 0.0690645376489, 0.0640603301571, 0.0605833800379, 0.0605627416962, 0.0621620046147, 0.057837075692, 0.0600957195598, 0.0528823330852, 0.0579430134835, 0.0558001484677, 0.0572766201561, 0.0584813099902, 0.051203450754};
float pca_vec2[100] = {-0.00810973111526, -0.00413846440217, 0.013270047417, 0.0728920624179, 0.0503041646435, -0.0573719544462, -0.0583390565392, -0.0425474897111, -0.0350381675736, -0.0262821225256, -0.0271450013199, -0.0357646318137, -0.0251363862425, -0.0220191852047, -0.0183793468832, -0.00537267919378, 0.00640871622994, -0.00677129135569, 0.00113215546801, 0.0180324760219, 0.0170789383183, 0.017553986305, 0.0329500390674, 0.0269133946407, 0.0308046893383, 0.0654758695008, 0.0504492713452, 0.061165711607, 0.0896119959387, 0.12807206923, 0.134954247463, 0.124298648229, 0.142993176681, 0.145248321901, 0.156919905078, 0.151691152354, 0.168524169673, 0.174819397346, 0.181628432363, 0.181356957915, 0.194905224772, 0.185797176163, 0.194193248166, 0.18523353091, 0.185653074894, 0.178877591693, 0.188543024019, 0.16547107554, 0.118043290828, 0.0950593770325, 0.0625244696896, 0.0766259451629, 0.0542732781717, 0.0489478173719, 0.0222286275205, 0.0396869321661, -0.0208687940135, -0.0225965118881, -0.0392904442982, -0.0393251756583, -0.0563060231356, -0.0774460862783, -0.0756428375926, -0.0768513224879, -0.0957480743546, -0.0951016070274, -0.102416998312, -0.104195465185, -0.117634547212, -0.111931968031, -0.119018123245, -0.119796307912, -0.131086889096, -0.120811143074, -0.116967526064, -0.125108405574, -0.127494846256, -0.1040505612, -0.113800673002, -0.114451435262, -0.113303427307, -0.110546980709, -0.113036951755, -0.110021424454, -0.11064168206, -0.0933976167341, -0.100314019581, -0.0836858289334, -0.0937428083342, -0.0802512002809, -0.0792029375739, -0.0733840133534, -0.0732975788848, -0.0626054881826, -0.0629049366523, -0.0600049708104, -0.0540299015187, -0.0638774404881, -0.0510464563274, -0.0509607421647};
float pca_vec3[100] = {0.0121627719024, -0.0240014509724, 0.000652262308148, 0.0166921419343, 0.00258326201162, -0.0795808226206, -0.136048236764, -0.0639292596517, -0.0620950950065, -0.0237851465367, 0.0217558803289, -0.0020874442428, 0.00937774941474, 0.016097412613, 0.0228235743201, 0.0206913044591, 0.0130344398321, 0.0232437920268, 0.00971399357331, 0.0214806594059, 0.0408408525972, 0.0136951964045, 0.0398278506947, 0.0178154870458, 0.0237458011624, -0.00173048518331, -0.0295273315177, -0.0473301516106, -0.0406435357338, -0.0106610467053, 0.00158513675823, 0.0367748924885, 0.0249774212719, 0.0465794804634, 0.0493057098666, 0.0552693896826, 0.0491888479683, 0.058746526622, 0.0250669533814, 0.0605197809723, 0.0827843881207, 0.0630945564047, 0.0485447244243, 0.0459195040327, 0.0156918913471, 0.0304039628393, 0.0452686828201, -0.00520095790678, -0.0750970146872, -0.117668100429, -0.145851523677, -0.161030876961, -0.180651112719, -0.150795543427, -0.19352871603, -0.155197154593, -0.193891339807, -0.192929121754, -0.174822963234, -0.171054758432, -0.182061989394, -0.18172355807, -0.166826399167, -0.152834809133, -0.154598793095, -0.133472351389, -0.131156902735, -0.101768539998, -0.110749817537, -0.0806621842707, -0.0452875681085, -0.0163640551175, -0.0207250627052, -0.00544111013114, 0.0274128577643, 0.0161825305963, 0.003945161886, 0.0511334973836, 0.0526941425282, 0.0543094412389, 0.0823050990118, 0.0784332294279, 0.0798485633051, 0.0885753825981, 0.0857112611099, 0.0986056597009, 0.108015159897, 0.114362850102, 0.129613634039, 0.117523947791, 0.140270314058, 0.15666922898, 0.159728184362, 0.168762064785, 0.165271034738, 0.176834549235, 0.169050465423, 0.168710554764, 0.181019000749, 0.181868234084};
float projected_mean_vec[3] = {-0.0377696440331, 0.0122326943969, -0.00683848313467};
float centroid1[3] = {-0.0575500233655, -0.00681151006894, 0.00355865119299};
float centroid2[3] = {0.0317095523771, -0.00566236902601, 0.0147106728489};
float centroid3[3] = {0.0204623297869, -0.0196426996875, -0.0127276159421};
float centroid4[3] = {0.00537814120145, 0.0321165787824, -0.00554170809985};

float* centroids[4] = {
  (float *) &centroid1, (float *) &centroid2,
  (float *) &centroid3, (float *) &centroid4
};

/*---------------------------*/
/*---------------------------*/
/*---------------------------*/

float result[SNIPPET_SIZE] = {0};
float proj1 = 0;
float proj2 = 0;
float proj3 = 0;

// Data array and index pointer
int re[SIZE] = {0};
volatile int re_pointer = 0;

/*---------------------------*/
/*       Norm functions      */
/*---------------------------*/

// Compute the L2 norm of (dim1, dim2) and centroid
// input: dim1: 1st dimension coordinate
//        dim2: 2nd dimension coordinate
//        centroid: size-2 array containing centroid coordinates
// output: L2 norm (Euclidean distance) between point and centroid
float l2_norm(float dim1, float dim2, float* centroid) {
  return sqrt(pow(dim1-centroid[0],2) + pow(dim2-centroid[1],2));
}

// Compute the L2 norm of (dim1, dim2, dim3) and centroid
// input: dim1: 1st dimension coordinate
//        dim2: 2nd dimension coordinate
//        dim3: 3rd dimension coordinate
//        centroid: size-3 array containing centroid coordinates
// output: L2 norm (Euclidean distance) between point and centroid
float l2_norm3(float dim1, float dim2, float dim3, float* centroid) {
  return sqrt(pow(dim1-centroid[0],2) + pow(dim2-centroid[1],2) + pow(dim3-centroid[2],2));
}

void setup(void) {
  Serial.begin(38400);

  pinMode(MIC_INPUT, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  re_pointer = 0;
  reset_blinker();
  setTimer();
}

void loop(void) {
  if (re_pointer == SIZE) {
    digitalWrite(RED_LED, LOW);

    // Apply enveloping function and get snippet with speech.
    // Do classification only if loud enough.
    if (envelope(re, result)) {

      // Reset projection result variables declared above
      proj1 = 0;
      proj2 = 0;
      proj3 = 0;

      /*---------------------------*/
      /*      CODE BLOCK PCA3      */
      /*---------------------------*/

      // Project 'result' onto the principal components
      // Hint: 'result' is an array
      // Hint: do this entire operation in 1 loop by replacing the '...'
      // YOUR CODE HERE
      for (int i = 0; i < SNIPPET_SIZE; i++) {
          proj1 += result[i]*pca_vec1[i];
          proj2 += result[i]*pca_vec2[i];
          proj3 += result[i]*pca_vec3[i];
      }

      // Demean the projection
      proj1 -= projected_mean_vec[0];
      proj2 -= projected_mean_vec[1];
      proj3 -= projected_mean_vec[2];

      // Classification
      // Use the function 'l2_norm' defined above
      // ith centroid: 'centroids[i]'
      float best_dist = 999999;
      int best_index = -1;
      // YOUR CODE HERE
      float arr[4] = {0, 0, 0, 0};
      for (int i = 0; i < 4; i++) {
          arr[i] = l2_norm3(proj1, proj2, proj3, centroids[i]);
          if (arr[i] < best_dist) {
            best_dist = arr[i];
            best_index = i;
          }
      }

      Serial.println("======");
      Serial.println(proj1);
      Serial.println(proj2);
      Serial.println(proj3);

      String arg[4] = {"cat", "green bean", "sadbears", "charlie"};
      if (best_dist < KMEANS_THRESHOLD) {
        Serial.println(arg[best_index]); 
      } else {
        Serial.println("The recording is noise");
      }

      // Compare 'best_dist' against the 'KMEANS_THRESHOLD' and print the result
      // If 'best_dist' is less than the 'KMEANS_THRESHOLD', the recording is a word
      // Otherwise, the recording is noise
      // YOUR CODE HERE


      /*---------------------------*/
      /*---------------------------*/
      /*---------------------------*/
    }
    else {
      Serial.println("Below LOUDNESS_THRESHOLD.");
    }


    delay(2000);
    re_pointer = 0;
  }
}

// Enveloping function with thresholding and normalizing,
// returns snippet of interest (containing speech)
bool envelope(int* data, float* data_out) {
  int32_t avg = 0;
  float maximum = 0;
  int32_t total = 0;
  int block;

  // Apply enveloping filter while finding maximum value
  for (block = 0; block < SIZE_AFTER_FILTER; block++) {
    avg = 0;
    for (int i = 0; i < 16; i++) {
      avg += data[i+block*16];
    }
    avg = avg >> 4;
    data[block] = abs(data[block*16] - avg);
    for (int i = 1; i < 16; i++) {
      data[block] += abs(data[i+block*16] - avg);
    }
    if (data[block] > maximum) {
      maximum = data[block];
    }
  }

  // If not loud enough, return false
  if (maximum < LOUDNESS_THRESHOLD) {
    return false;
  }

  // Determine threshold
  float thres = THRESHOLD * maximum;

  // Figure out when interesting snippet starts and write to data_out
  block = PRELENGTH;
  while (data[block++] < thres);
  if (block > SIZE_AFTER_FILTER - SNIPPET_SIZE) {
    block = SIZE_AFTER_FILTER - SNIPPET_SIZE;
  }
  for (int i = 0; i < SNIPPET_SIZE; i++) {
    data_out[i] = data[block-PRELENGTH+i];
    total += data_out[i];
  }

  // Normalize data_out
  for (int i = 0; i < SNIPPET_SIZE; i++) {
    data_out[i] = data_out[i] / total;
  }

  return true;
}

/*---------------------------*/
/*     Helper functions      */
/*---------------------------*/

void reset_blinker(void) {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(GREEN_LED, LOW);
}

/*---------------------------*/
/*    Interrupt functions    */
/*---------------------------*/

// ISR for timestep
#pragma vector=TIMER2_A0_VECTOR    // Timer A ISR
__interrupt void Timer2_A0_ISR(void) {
  if (re_pointer < SIZE) {
    digitalWrite(RED_LED, HIGH);
    re[re_pointer] = (analogRead(MIC_INPUT) >> 4) - 128;
    re_pointer += 1;
  }
}

// Set timer for timestep; use A2 since A0 & A1 are used by PWM
void setTimer(void) {
  // Set the timer based on 25MHz clock
  TA2CCR0 = (unsigned int) (25000*ADC_TIMER_MS);
  TA2CCTL0 = CCIE;
  __bis_SR_register(GIE);
  TA2CTL = TASSEL_2 + MC_1 + TACLR + ID_0;
}
