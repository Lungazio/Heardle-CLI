#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <cstdlib>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <cctype>


const int AUDIO_FREQUENCY = 44100;
const int AUDIO_CHANNELS = 2;
const int AUDIO_CHUNK_SIZE = 2048;

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNK_SIZE) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void quitSDL() {
    Mix_CloseAudio();
    SDL_Quit();
}

std::vector<std::string> readSongNames(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> songNames;
    std::string line;
    while (std::getline(file, line)) {
        songNames.push_back(line);
    }
    file.close();
    return songNames;
}

std::string getRandomSong(const std::vector<std::string>& songNames, const std::unordered_set<std::string>& playedSongs) {
    std::random_device randomDevice;
    std::mt19937 randomEngine(randomDevice());
    std::uniform_int_distribution<size_t> distribution(0, songNames.size() - 1);

    std::string title;
    bool isTitlePlayed = true;

    while (isTitlePlayed) {
        title = songNames[distribution(randomEngine)];
        isTitlePlayed = playedSongs.count(title) > 0;
    }

    return title;
}

void downloadAudio(const std::string& songTitle) {
    #ifdef _WIN32
        std::string nullOutput = " > NUL 2>&1";
    #else
        std::string nullOutput = " > /dev/null 2>&1";
    #endif

    std::string command = "youtube-dl -x --audio-format mp3 --audio-quality 9 --output 'audio.%(ext)s' 'ytsearch1:" + songTitle + "'" + nullOutput;
    std::system(command.c_str());
}


void deleteAudioFile() {
    std::remove("audio.mp3");
}

void displayGameScreen(bool showText1, bool showText2, bool showText3, bool showText4, bool showText5) {
    std::cout << "####################" << std::endl;
    std::cout << "#      Heardle     #" << std::endl;
    std::cout << "####################" << std::endl;
    if (showText1) {
        std::cout << "#  1. 0.5 seconds  #" << std::endl;
    }
    if (showText2) {
        std::cout << "#  2. 1.5 seconds  #" << std::endl;
    }
    if (showText3) {
        std::cout << "#  3. 3 seconds    #" << std::endl;
    }
    if (showText4) {
        std::cout << "#  4. 5 seconds    #" << std::endl;
    }
    if (showText5) {
        std::cout << "#  5. 8 seconds    #" << std::endl;
    }
    std::cout << "####################" << std::endl;
}

std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return lowerStr;
}


int main(int argc, char* argv[]) {
    if (!initializeSDL()) {
        return 1;
    }


    

     bool guessedSong = false;
        while (!guessedSong) {
        // Select a random song that hasn't been played yet
        
        // Add the played song to the set
        // auto randomSong = getRandomSong(songNames, playedSongs);


        // playedSongs.insert(randomSong);
            std::cout << "Sound initialization successful!" << std::endl;

    // Read song names from songs.txt
        auto songNames = readSongNames("songs.txt");

        // Set to keep track of played songs
        std::unordered_set<std::string> playedSongs;
        auto randomSong = getRandomSong(songNames, playedSongs);
    

        // Download the audio for the random song
        std::cout << "Loading game..." << std::endl;
        downloadAudio(randomSong);
        playedSongs.insert(randomSong);

        // Load the audio file
        Mix_Music *music = Mix_LoadMUS("audio.mp3");

        // Show the game screen
        displayGameScreen(true, false, false, false, false);

        // Wait for user input before playing the first snippet
        std::cout << "Press enter to play the first snippet" << std::endl;
        std::cin.ignore();
        

        // Play the first snippet
        displayGameScreen(true, false, false, false, false);
        Mix_PlayMusic(music, 1);
        SDL_Delay(750);
        Mix_HaltMusic();
        

        // Allow the user to repeat the first snippet
        bool repeatFirstSnippet = true;
        while (repeatFirstSnippet && !guessedSong) {
            std::cout << "Press 'r' to repeat the snippet, 'p' to play the next snippet, or guess the song:" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            if (input[0] == 'r' || input[0] == 'R') {
                Mix_PlayMusic(music, 1);
                SDL_Delay(750);
                Mix_HaltMusic();
                displayGameScreen(true, false, false, false, false);
            } else if (input[0] == 'p' || input[0] == 'P'){
                repeatFirstSnippet = false;
            } else {
                std::string guess = toLower(input);
                if (guess == toLower(randomSong)) {
                    std::cout << "Congratulations, you guessed the song!It was: " +randomSong  << std::endl;
                    guessedSong = true;
                    break;
                } else {
                    std::cout << "Incorrect guess. Moving on to the next snippet." << std::endl;
                    repeatFirstSnippet = false; // Break out of the while loop
                }
            }
        }
        if(guessedSong){
            break;
        }



       // Play the second snippet
       displayGameScreen(true, true, false, false, false);
        Mix_PlayMusic(music, 1);
        SDL_Delay(1500);
        Mix_HaltMusic();
        

         // Allow the user to repeat the first snippet
        bool repeatSecondSnippet = true;
        while (repeatSecondSnippet && !guessedSong) {
            std::cout << "Press 'r' to repeat the snippet, 'p' to play the next snippet, or guess the song:" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            if (input[0] == 'r' || input[0] == 'R') {
                Mix_PlayMusic(music, 1);
                SDL_Delay(750);
                Mix_HaltMusic();
                displayGameScreen(true, true, false, false, false);
            } else if (input[0] == 'p' || input[0] == 'P') {
                repeatSecondSnippet = false;
            } else {
                std::string guess = toLower(input);
                if (guess == toLower(randomSong)) {
                    std::cout << "Congratulations, you guessed the song!It was: " +randomSong  << std::endl;
                    guessedSong = true;
                    break;
                } else {
                    std::cout << "Incorrect guess. Moving on to the next snippet." << std::endl;
                    repeatSecondSnippet = false; // Break out of the while loop
                }
            }
        }
        if(guessedSong){
            break;
        }
        // Play the third snippet
        displayGameScreen(true, true, true, false, false);
        Mix_PlayMusic(music, 1);
        SDL_Delay(3000);
        Mix_HaltMusic();
        

        // Allow the user to repeat the third snippet
        bool repeatThirdSnippet = true;
        while (repeatThirdSnippet && !guessedSong) {
            std::cout << "Press 'r' to repeat the snippet, 'p' to play the next snippet, or guess the song:" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            if (input[0] == 'r' || input[0] == 'R') {
                Mix_PlayMusic(music, 1);
                SDL_Delay(1500);
                Mix_HaltMusic();
                displayGameScreen(true, false, false, false, false);
            } else if (input[0] == 'p' || input[0] == 'P') {
                repeatThirdSnippet = false;
            } else {
                std::string guess = toLower(input);
                if (guess == toLower(randomSong)) {
                    std::cout << "Congratulations, you guessed the song!It was: " +randomSong  << std::endl;
                    guessedSong = true;
                    break;
                } else {
                    std::cout << "Incorrect guess. Moving on to the next snippet." << std::endl;
                    repeatThirdSnippet = false; // Break out of the while loop
                }
            }
        }
        if(guessedSong){
            break;
        }
        /// Play the forth snippet
        displayGameScreen(true, true, true, true, false);
        Mix_PlayMusic(music, 1);
        SDL_Delay(5000);
        Mix_HaltMusic();
        

        // Allow the user to repeat the fourth snippet
        bool repeatFourthSnippet = true;
        while (repeatFourthSnippet && !guessedSong) {
            std::cout << "Press 'r' to repeat the snippet, 'p' to play the next snippet, or guess the song:" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            if (input[0] == 'r' || input[0] == 'R'){
                Mix_PlayMusic(music, 1);
                SDL_Delay(5000);
                Mix_HaltMusic();
                displayGameScreen(true, false, false, false, false);
            } else if (input[0] == 'p' || input[0] == 'P') {
                repeatFourthSnippet = false;
            } else {
                std::string guess = toLower(input);
                if (guess == toLower(randomSong)) {
                    std::cout << "Congratulations, you guessed the song! It was: " +randomSong << std::endl;
                    guessedSong = true;
                    break;
                } else {
                    std::cout << "Incorrect guess. Moving on to the next snippet." << std::endl;
                    repeatFourthSnippet = false; // Break out of the while loop
                }
            }
        }
        if(guessedSong){
            break;
        }


     /// Play the fifth snippet 
        displayGameScreen(true, true, true, true, true);
        Mix_PlayMusic(music, 1);
        SDL_Delay(8000);
        Mix_HaltMusic();
        

        // Allow the user to repeat the fifth snippet
        bool repeatLastSnippet = true;
        while (repeatLastSnippet && !guessedSong) {
            std::cout << "Press 'r' to repeat the snippet, 'p' to give up, or guess the song" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            if (input[0] == 'r' || input[0] == 'R') {
                Mix_PlayMusic(music, 1);
                SDL_Delay(8000);
                Mix_HaltMusic();
                displayGameScreen(true, false, false, false, false);
            } else if (input[0] == 'p' || input[0] == 'P') {
                std::cout << "You lose the answer was: " + randomSong << std::endl;
                repeatLastSnippet = false;
            } else {
                std::string guess = toLower(input);
                if (guess == toLower(randomSong)) {
                    std::cout << "Congratulations, you guessed the song! Phew! It was: " +randomSong  << std::endl;
                    guessedSong = true;
                    break;
                } else {
                    std::cout << "Incorrect guess. You lose the answer was: " + randomSong << std::endl;
                    repeatLastSnippet = false; // Break out of the while loop
                }
            }
        }


    Mix_FreeMusic(music);

    // Delete the audio file after the game is finished
    

    // Ask the user if they want to play again
    break;
    // Clear the played songs set if the user wants to play again
    playedSongs.clear();
}
deleteAudioFile();
quitSDL();
return 0;
}
