#pragma once

#include <string>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <array>


// TODO: ADD HELPER FUNCTIONS TO PRINT OUT PROVIDED MODEL INFORMATION
class ModelHandler
{
public:
    //! This default parameters are specialized for personel usage. if you insist to use it please provide proper shape for your model
    ModelHandler(const std::string &modelPath) :
    _env(ORT_LOGGING_LEVEL_WARNING, "ModelHandler"), 
    _session(_env, modelPath.c_str(), Ort::SessionOptions{})
    {
        auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
        _inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, _input.data(), _input.size(), _inputShape.data(), _inputShape.size());
        _outputTensor = Ort::Value::CreateTensor<float>(memoryInfo, _results.data(), _results.size(), _outputShape.data(), _outputShape.size());
    };
    ~ModelHandler() = default;

    std::ptrdiff_t prepareInputTensorAndPredict(cv::Mat &image, int sizeX = 32, int sizeY = 32){
        cv::cvtColor(image, image, cv::COLOR_RGB2GRAY);
        cv::resize(image, image, cv::Size(sizeX, sizeY));
        image = image.reshape(1, 1); // reshape image input 1D
        image.convertTo(_input, CV_32FC1, 1.0f / 255); // normalize input

        try{
            _session.Run(_runOptions, _inputName.data(), &_inputTensor, 1, _outputName.data(), &_outputTensor, 1);
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }

        // std::vector<std::pair<size_t, float>> indexValuePairs;
        // for (size_t i = 0; i < _results.size(); ++i) {
        //     indexValuePairs.emplace_back(i, _results[i]);
        // }
        // std::sort(indexValuePairs.begin(), indexValuePairs.end(), [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        // // show Top5
        // for (size_t i = 0; i < 4; ++i) {
        //     const auto& result = indexValuePairs[i];
        //     // std::cout << i + 1 << ": " << labels[result.first] << " " << result.second << std::endl;
        // }

        // sort results
        _result = std::distance(_results.begin(), std::max_element(_results.begin(), _results.end()));
        return _result;
    }

private:
    Ort::Env _env;
    Ort::RunOptions _runOptions;
    Ort::Session _session{nullptr}; // will be filled later on
    // NOTE: PLEASE PROVIDE YOUR OWN INPUT AND OUTPUT NAMES HERE.
    // FIXME: MAKE IT WORK WITH EVERY MODEL. THIS IS FOR PERSONAL USAGE
    std::vector<const char*> _inputName {"sign"};
    std::vector<const char*> _outputName {"output"};
    
    // std::int64_t numChannels = 1;
    static constexpr std::int64_t _width = 32;
    static constexpr std::int64_t _height = 32;
    static constexpr std::int64_t _numClasses = 4;

    std::array<float, _width * _height> _input{};
    std::array<float, _numClasses> _results{};
    
    std::int64_t _result{0};

    Ort::Value _inputTensor{nullptr};
    std::array<std::int64_t,4> _inputShape{1, _width, _height, 1};
    
    Ort::Value _outputTensor{nullptr};
    std::array<std::int64_t,2> _outputShape{1, _numClasses};
};

inline std::vector<float> loadImage(const std::string& filename, int sizeX = 32, int sizeY = 32)
{
    cv::Mat image = cv::imread(filename);
    if (image.empty()) {
        std::cout << "No image found.";
    }

    // convert from BGR to RGB
    cv::cvtColor(image, image, cv::COLOR_RGB2GRAY);
    //! if your model needs it use this instead
    // cv::cvtColor(image, image, cv::COLOR_BGR2RGB); 
    // resize
    cv::resize(image, image, cv::Size(sizeX, sizeY));
    // reshape to 1D
    image = image.reshape(1, 1);

    std::vector<float> vec;
    image.convertTo(vec, CV_32FC1, 1. / 255);

    // uint_8, [0, 255] -> float, [0, 1]
    // Normalize number to between 0 and 1
    // Convert to vector<float> from cv::Mat.
    // NOTE: consider this if your models needs to group according to it's channel
    // Transpose (Height, Width, Channel)(224,224,3) to (Chanel, Height, Width)(3,224,224)
    // std::vector<float> output;
    // for (size_t ch = 0; ch < 3; ++ch) {
    //     for (size_t i = ch; i < vec.size(); i += 3) {
    //         output.emplace_back(vec[i]);
    //     }
    // }
    return vec;
}

inline std::vector<std::string> loadLabels(const std::string& filename)
{
    std::vector<std::string> output;

    std::ifstream file(filename);
    if (file) {
        std::string s;
        while (getline(file, s)) {
            output.emplace_back(s);
        }
        file.close();
    }

    return output;
}

// TODO: MOVE LABELS TO ANOTHER .TXT FILE AND LOAD FROM THERE
// NOTE: this is for debugging and personal use only. pls specify you own labels.
// FIXME: This may throw out of bound exception if not used very carefully.
inline std::vector<std::string> modelLabels{"stop","turn left", "turn right", "keep forward"};

void resultToLabel(std::ptrdiff_t *result){
    std::cout << "predicted -> " << modelLabels[*result] << std::endl;
}

void testModelHandler(const std::string& path){
    ModelHandler mHAndler(path);
    cv::Mat image = cv::imread("/home/seyyit/projects/cpp-autonomous-vehicle/left.png");
    auto result = mHAndler.prepareInputTensorAndPredict(image);
    resultToLabel(&result);
    // TODO: implement this
    // std::vector<std::pair<size_t, float>> indexValuePairs;
    // for (size_t i = 0; i < results.size(); ++i) {
    //     indexValuePairs.emplace_back(i, results[i]);
    // }
    // std::sort(indexValuePairs.begin(), indexValuePairs.end(), [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

    // // show Top5
    // for (size_t i = 0; i < 4; ++i) {
    //     const auto& result = indexValuePairs[i];
    //     std::cout << i + 1 << ": " << labels[result.first] << " " << result.second << std::endl;
    // }
}