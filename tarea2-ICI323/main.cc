#include "global.hh"

std::mutex mtx;  // Mutex para controlar el acceso al mapa de histograma

void countWords(const std::vector<std::string>& words, std::map<std::string, int>& histogram) {
    for (const auto& word : words) {
        std::lock_guard<std::mutex> lock(mtx);
        histogram[word]++;
    }
}

int main(int argc, char *argv[]) {
    int threads = 1;
    std::string filename;

    int opt;
    while ((opt = getopt(argc, argv, "t:f:h")) != -1) {
        switch (opt) {
        case 't':
            threads = std::stoi(optarg);
            break;
        case 'f':
            filename = optarg;
            break;
        case 'h':
            std::cout << "Modo de uso: " << argv[0]
                      << " -t N -f FILENAME [-h]" << std::endl;
            return 0;
        default:
            std::cerr << "Uso incorrecto. Usa -h para ayuda." << std::endl;
            return 1;
        }
    }

    std::ifstream file(filename);  // Abre el archivo
    if (!file) {
        std::cerr << "No se pudo abrir el archivo " << filename << std::endl;
        return 1;
    }

    std::string word;
    std::vector<std::string> words;

    while (file >> word) {
        words.push_back(word);
    }

    std::map<std::string, int> histogram;
    std::vector<std::thread> threadList;
    int wordsPerThread = words.size() / threads;

    for (int i = 0; i < threads; ++i) {
        int start = i * wordsPerThread;
        int end = (i == threads - 1) ? words.size() : (i + 1) * wordsPerThread;
        std::vector<std::string> subWords(words.begin() + start, words.begin() + end);
       
        threadList.push_back(std::thread(countWords, subWords, std::ref(histogram)));
    }

    // Espera a que todos los threads terminen
    for (auto& th : threadList) {
        th.join();
    }

    // Imprime el histograma
    for (const auto& pair : histogram) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}


