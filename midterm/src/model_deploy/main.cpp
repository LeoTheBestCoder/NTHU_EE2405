#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"
#include "DA7212.h"
#include "uLCD_4DGL.h"
#include <cmath>

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#define bufferLength (32)
#define signalLength (42)

DA7212 audio;
InterruptIn pause_button(SW2);
InterruptIn confirm_button(SW3);
uLCD_4DGL uLCD(D1, D0, D2);
Serial pc(USBTX, USBRX);

// in this project, sw2 is mode setting, sw3 is confirm
Thread t1;  // for music
Thread t2(osPriorityNormal, 120 * 1024);  // for gesture
Thread t3;  // for uLCD
int pause = 0, confirm = 0, another_confirm = 0;
int state = 1;
float signal_sended[42];
char serialInBuffer[bufferLength];
int serialCount = 0;
// 1 denotes the state of playing songs
// 2 denotes the state of selecting mode
// 3 denotes the state of selecting songs from my song list
// 4 denotes the game state
// 5 denotes the loading state

int idx_of_song = 2; // ranging from 1 ~ 3
int mode_selection_cursor = 1003;
int song_select_cursor = 1000;
int gesture_idx = -1; // 0 denotes ring, and 1 denotes slope
bool left_effective_area = false, right_effective_area = false;
bool got_score = false;
int score = 0;

int16_t waveform[kAudioTxBufferSize];
int song1[42] = {
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261,
  392, 392, 349, 349, 330, 330, 294,
  392, 392, 349, 349, 330, 330, 294,
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261};

int noteLength1[42] = {
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2};

int song2[39] = {
  261, 392, 349, 329, 349, 329, 
  349, 329, 329, 0, 329, 523, 
  493, 493, 493, 523, 329, 293, 
  261, 0, 261, 523, 493, 523, 
  493, 523, 523, 493, 523, 0, 
  440, 392, 349, 329, 349, 329, 
  349, 329, 261};
int noteLength2[39] = {
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1
  };

int song3[34] = {
  523, 587, 659, 523, 440, 440, 0, 
  587, 493, 392, 329, 329, 0, 493, 
  440, 392, 261, 392, 329, 0, 293, 
  329, 349, 349, 392, 349, 392, 329, 
  392, 523, 659, 587, 523, 523};
int noteLength3[34] = {
  1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 1, 1};

void playNote(int freq)
{
  for(int i = 0; i < kAudioTxBufferSize; i++)
  {
    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
  }
  audio.spk.play(waveform, kAudioTxBufferSize);
}
void music() {
  while(1) {
    if(pause == 0 || state == 4){
      if(idx_of_song == 1){
        for(int i = 0; i < 42; i++)
        {
          int length = noteLength1[i];
          if (pause == 1) {
              break;
            }
          while(length--)
          {
            

            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize / 3; ++j)
            {
              // queue.call(playNote, song[i]);
              playNote(song1[i]);
            }
            //  if(length < 1) wait(1.0);
            if(length < 1) playNote(30000);
          }
        }
      }
      else if(idx_of_song == 2){
        for(int i = 0; i < 39; i++)
        {
          int length = noteLength2[i];
          if (pause == 1) {
              break;
            }
          while(length--)
          {
            
            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize / 4; ++j)
            {
              // queue.call(playNote, song[i]);
              playNote(song2[i]);
            }
            //  if(length < 1) wait(1.0);
            if(length < 1) playNote(30000);
            if(pause == 1){
              state = 2;
            }
          }
        }
      }
      else if(idx_of_song == 3){
        for(int i = 0; i < 34; i++)
        {
          int length = noteLength3[i];
          if (pause == 1) {
              break;
            }
          while(length--)
          {
            
            // the loop below will play the note for the duration of 1s
            for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize / 4; ++j)
            {
              // queue.call(playNote, song[i]);
              playNote(song3[i]);
            }
            //  if(length < 1) wait(1.0);
            if(length < 1) playNote(30000);
          }
        }
      }
    }
    else if(pause == 1){
      playNote(30000);
    }
  }
}
// Return the result of the last prediction
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;

  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }

  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }
  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;

  return this_predict;
}

void gesture() {
  

  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  constexpr int kTensorArenaSize = 60 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];

  // Whether we should clear the buffer next time we fetch data
  bool should_clear_buffer = false;
  bool got_data = false;

  // The gesture index of the prediction
  int gesture_index;

  // Set up logging.
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return -1;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroOpResolver<6> micro_op_resolver;
  micro_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                               tflite::ops::micro::Register_RESHAPE(), 1);

  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  TfLiteTensor* model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return -1;
  }

  int input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return -1;
  }

  error_reporter->Report("Set up successful...\n");

  while (true) {
    // Attempt to read new data from the accelerometer
    got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                 input_length, should_clear_buffer);

    // If there was no new data,
    // don't try to clear the buffer again and wait until next time
    if (!got_data) {
      should_clear_buffer = false;
      continue;
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      error_reporter->Report("Invoke failed on index: %d\n", begin_index);
      continue;
    }

    // Analyze the results to obtain a prediction
    gesture_index = PredictGesture(interpreter->output(0)->data.f);

    // Clear the buffer next time we read data
    should_clear_buffer = gesture_index < label_num;

    // Produce an output
    if (gesture_index < label_num) {
      error_reporter->Report(config.output_message[gesture_index]);
      gesture_idx = gesture_index;
      if(gesture_idx == 0){ // ring
        mode_selection_cursor++; 
        song_select_cursor++;
        if(left_effective_area && !got_score){
          score += 5;
          got_score = true;
        }
      }
      else if(gesture_idx == 1){  // slope
         mode_selection_cursor--;    
         song_select_cursor--;
         if(right_effective_area && !got_score){
           score += 5;
           got_score = true;
         }
      }
    }
  }
  
}


void loadSignal(void)
{
  // green_led = 0;
  int i = 0;
  serialCount = 0;
  // audio.spk.pause();
  while(i < signalLength)
  {
    if(pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 5)
      {
        serialInBuffer[serialCount] = '\0';
        signal_sended[i] = (float) atof(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
  }
  for(int j = 0; j < 42; j++){
    song1[j] = signal_sended[j];
  }
  state = 1;
  pause = 0;
  idx_of_song = 1;
  confirm = 0;
  // green_led = 1;
}

void display() {
  int x1, y1, x2, y2;
  while(1) {
    if (state == 1 && pause == 0){
      uLCD.cls();
      // uLCD.locate(0, 0);
      if(idx_of_song == 1){
        // uLCD.cls();
        uLCD.color(GREEN);
        uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);
        uLCD.printf("song 1\n");
        uLCD.printf("Little Star\n");
      }
      else if(idx_of_song == 2){
        // uLCD.cls();
        uLCD.color(GREEN);
        uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);
        uLCD.printf("song 2\n");
        uLCD.printf("Pi Sing Dai Yueh \nDe Siang Ni\n");
      }
      else if(idx_of_song == 3){
        // uLCD.cls();
        uLCD.color(GREEN);
        uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);
        uLCD.printf("song 3\n");
        uLCD.printf("Lemon\n");
      }
      if(pause == 1){
        state = 2;
      }
    }
    else if(state == 2){
      if(mode_selection_cursor % 5 + 1 == 1){
        uLCD.cls();
        uLCD.color(RED);
        uLCD.printf("1. Forward <---\n");
        uLCD.color(GREEN);
        uLCD.printf("2. Backward --->\n");
        uLCD.printf("3. Change Songs..\n");
        uLCD.printf("4. Game Mode\n");
        uLCD.printf("5. Load song\n");
        if(confirm == 1) {  // confirm is pressed
          if(idx_of_song > 1){
            pause = 0;
            idx_of_song--;
            confirm = 0;
            state = 1;
            another_confirm = 0;
          }
          else{
            pause = 0;
            idx_of_song = 3;
            confirm = 0;
            state = 1;
            another_confirm = 0;
          }
        }
      }
      else if(mode_selection_cursor % 5 + 1 == 2){
        uLCD.cls();
        uLCD.color(GREEN);
        uLCD.printf("1. Forward <---\n");
        uLCD.color(RED);
        uLCD.printf("2. Backward --->\n");
        uLCD.color(GREEN);
        uLCD.printf("3. Change Songs..\n");
        uLCD.printf("4. Game Mode\n");
        uLCD.printf("5. Load song\n");
        if(confirm == 1) {  // confirm is pressed
          if(idx_of_song == 3){
            pause = 0;
            idx_of_song = 1;
            confirm = 0;
            state = 1;
            another_confirm = 0;
          }
          else{
            pause = 0;
            idx_of_song++;
            confirm = 0;
            state = 1;
            another_confirm = 0;
          }
        }
      }
      else if(mode_selection_cursor % 5 + 1 == 3){
        if(confirm != 1){
          uLCD.cls();
          uLCD.color(GREEN);
          uLCD.printf("1. Forward <---\n");
          uLCD.printf("2. Backward --->\n");
          uLCD.color(RED);
          uLCD.printf("3. Change Songs..\n");
          uLCD.color(GREEN);
          uLCD.printf("4. Game Mode\n");
          uLCD.printf("5. Load song\n");
        }
        if(confirm == 1) { // confirm is pressed
          state = 3;
          confirm = 0;
        }
      }
      else if(mode_selection_cursor % 5 + 1 == 4){
        uLCD.cls();
        uLCD.color(GREEN);
        uLCD.printf("1. Forward <---\n");
        uLCD.printf("2. Backward --->\n");
        uLCD.printf("3. Change Songs..\n");
        uLCD.color(RED);
        uLCD.printf("4. Game Mode\n");
        uLCD.color(GREEN);
        uLCD.printf("5. Load song\n");
        if(confirm == 1){
          pause = 0;
          state = 4;
          confirm = 0;
          idx_of_song = 1;
        }
      }
      else if(mode_selection_cursor % 5 + 1 == 5){
        uLCD.cls();
        uLCD.color(GREEN);
        uLCD.printf("1. Forward <---\n");
        uLCD.printf("2. Backward --->\n");
        uLCD.printf("3. Change Songs..\n");
        uLCD.printf("4. Game Mode\n");
        uLCD.color(RED);
        uLCD.printf("5. Load song\n");
        if(confirm == 1){
          pause = 0;
          state = 5;
          confirm = 0;
          idx_of_song = 1;
        }
      }
    }
    else if (state == 3){
      if (song_select_cursor %3 + 1 == 1){
            if (confirm != -1){
              uLCD.cls();
              uLCD.color(GREEN);
              uLCD.printf("Choose a song from the song list: \n");
              uLCD.color(BLUE);
              uLCD.printf("1. Little Star\n");
              uLCD.color(GREEN);
              uLCD.printf("2. Pi Sing Dai Yueh De Siang Ni\n");
              uLCD.printf("3. Lemon\n");
              uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);
              
            }
            else if(confirm == -1){
                pause = 0;
                idx_of_song = 1;
                confirm = 0;
                state = 1;
                // break;
                // another_confirm = 0;
            }
          }
          else if(song_select_cursor % 3 + 1 == 2){
            state = 3;
            if(confirm != -1){
              uLCD.cls();
              uLCD.color(GREEN);
              uLCD.printf("Choose a song from the song list: \n");
              uLCD.printf("1. Little Star\n");
              uLCD.color(BLUE);
              uLCD.printf("2. Pi Sing Dai Yueh De Siang Ni\n");
              uLCD.color(GREEN);
              uLCD.printf("3. Lemon\n");
              uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);

              
            }
            if(confirm == -1){
                pause = 0;
                idx_of_song = 2;
                confirm = 0;
                state = 1;
                // break;
                // another_confirm = 0;
            }
          }
          else if(song_select_cursor % 3 + 1 == 3){
            state = 3;
            if (confirm != -1){
              uLCD.cls();
              uLCD.color(GREEN);
              uLCD.printf("Choose a song from the song list: \n");
              uLCD.printf("1. Little Star\n");
              uLCD.printf("2. Pi Sing Dai Yueh De Siang Ni\n");
              uLCD.color(BLUE);
              uLCD.printf("3. Lemon\n");
              uLCD.color(GREEN);
              uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);

            }
            if(confirm == -1){
                pause = 0;
                idx_of_song = 3;
                confirm = 0;
                state = 1;
                // break;
                // another_confirm = 0;
            }
          }
    }
    else if(state == 4){
      // while (1) {
        if(pause == 1){
          state = 1;
          idx_of_song = 1;
          pause = 0;
          confirm = 0;
          // break;
        }
        else if(pause == 0){
          x1 = 20;
          y1 = 0;
          x2 = 100;
          y2 = 0;
          for(int i=0; i<20; i++){
            if(i == 0 && got_score == true){
              got_score = false;
            }
            // if(pause == 1){
            //   state = 1;
            //   idx_of_song = 1;
            //   pause = 0;
            //   confirm = 0;
            //   break;
            // }
            uLCD.cls();
            uLCD.color(GREEN);
            uLCD.printf("Game Mode\n");
            uLCD.printf("Your Score: %d\n", score);
            uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);
            uLCD.line(0, 80, 140, 80, WHITE);
            uLCD.line(0, 100, 140, 100, WHITE);
            uLCD.circle(x1, y1, 10, RED);
            wait_us(100);
            if(y1 >= 80 && y1 <= 100){
              left_effective_area = true;
            }
            else{
              left_effective_area = false;
            }
            y1 += 6;
          }
          for(int i=0; i<20; i++){
            if(i == 0 && got_score == true){
              got_score = false;
            }
            // if(pause == 1){
            //   state = 1;
            //   idx_of_song = 1;
            //   pause = 0;
            //   confirm = 0;
            //   break;
            // }
            uLCD.cls();
            uLCD.color(GREEN);
            uLCD.printf("Game Mode\n");
            uLCD.printf("Your Score: %d\n", score);
            uLCD.printf("state = %d\npause = %d\nconfirm = %d\n", state, pause, confirm);
            uLCD.line(0, 80, 140, 80, WHITE);
            uLCD.line(0, 100, 140, 100, WHITE);
            uLCD.circle(x2, y2, 10, BLUE);
            wait_us(100);
            if(y2 >= 80 && y2 <= 100){
              right_effective_area = true;
            }
            else{
              right_effective_area = true;
            }
            y2 += 6;
          }
        }
      // }
    }
    else if(state == 5){
      uLCD.cls();
      uLCD.color(GREEN);
      uLCD.printf("loading song from PC\n");
      loadSignal();
      // if(pause == 1){
      //   state = 1;
      //   idx_of_song = 1;
      //   pause = 1;
      //   confirm = 0;
      // }
    }
  }
}


void set_pause() {
  pause = 1;
}

void set_confirm() {
  if(state == 3){
    confirm = -1;
  }
  else{
    confirm = 1;
  }
}

int main(){
  pause_button.rise(set_pause);
  confirm_button.rise(set_confirm);
  t1.start(music);
  t2.start(gesture);
  t3.start(display);
}