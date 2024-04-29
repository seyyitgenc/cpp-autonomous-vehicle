#pragma once


#include <string>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <array>

#include "utils/filesystem.hpp"

enum struct SIGN{
    STOP = 0,
    TURN_LEFT = 1,
    TURN_RIGHT = 2,
    KEEP_FORWARD = 3,
    GATE1 = 4,
    GATE2 = 5,
    GATE3 = 6,
    NONE = -1
};

std::string print_shape(const std::vector<std::int64_t>& v) {
  std::stringstream ss("");
  for (std::size_t i = 0; i < v.size() - 1; i++) ss << v[i] << "x";
  ss << v[v.size() - 1];
  return ss.str();
}

struct ModelInformation
{
    static constexpr std::int64_t width = 32;
    static constexpr std::int64_t height = 32;
    static constexpr std::int64_t numClasses = 8;

    Ort::Value inputTensor{nullptr};
    std::array<std::int64_t,4> inputShape{1, width, height, 1};
    
    Ort::Value outputTensor{nullptr};
    std::array<std::int64_t,2> outputShape{1, numClasses};
    
    std::vector<const char*> inputNames{"sign"};
    std::vector<const char*> outputNames{"output"};

    std::array<float, width * height> input{};
    std::array<float, numClasses> results{};
};
void test(){
}
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
        _info.inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, _info.input.data(), _info.input.size(), _info.inputShape.data(), _info.inputShape.size());
        _info.outputTensor = Ort::Value::CreateTensor<float>(memoryInfo, _info.results.data(), _info.results.size(), _info.outputShape.data(), _info.outputShape.size());
        // _info.labels = loadLabels(FileSystem::getPath("src/model/labels.txt"));
    };
    ~ModelHandler() = default;

    std::ptrdiff_t prepareInputTensorAndPredict(cv::Mat &image, int sizeX = 32, int sizeY = 32){
        cv::cvtColor(image, image, cv::COLOR_RGB2GRAY);
        cv::resize(image, image, cv::Size(sizeX, sizeY));
        cv::imshow("sign" ,image);
        image = image.reshape(1, 1); // reshape image input 1D
        image.convertTo(_info.input, CV_32FC1); // normalize input

        try{
            _session.Run(_runOptions, _info.inputNames.data(), &_info.inputTensor, 1, _info.outputNames.data(), &_info.outputTensor, 1);
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }

        // sort results
        std::vector<std::pair<size_t, float>> indexValuePairs;
        for (size_t i = 0; i < _info.results.size(); ++i) {
            indexValuePairs.emplace_back(i, _info.results[i]);
        }
        std::sort(indexValuePairs.begin(), indexValuePairs.end(), [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        // show Top5
        for (size_t i = 0; i < 5; ++i) {
            const auto& result = indexValuePairs[i];
            std::cout << "index "  << result.first << " with prob. " << result.second << std::endl;
            // std::cout << i + 1 << ": " << labels[result.first] << " " << result.second << std::endl;
        }

        _result = std::distance(_info.results.begin(), std::max_element(_info.results.begin(), _info.results.end()));
        return _result;
    }

private:
    Ort::Env _env;
    Ort::RunOptions _runOptions;
    Ort::Session _session{nullptr}; // will be filled later on
    // NOTE: PLEASE PROVIDE YOUR OWN INPUT AND OUTPUT NAMES HERE.
    // FIXME: MAKE IT WORK WITH EVERY MODEL. THIS IS FOR PERSONAL USAGE
    
    std::int64_t _result{0};

    ModelInformation _info;
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

// NOTE: this is for debugging and personal use only. pls specify you own labels.
// FIXME: This may throw out of bound exception if not used very carefully.
inline std::vector<std::string> modelLabels{"stop","turn left", "turn right", "keep forward", "other","1 gate","2 gate","3 gate"};

inline void resultToLabel(std::ptrdiff_t *result){
    std::cout << "predicted -> " << modelLabels[*result] << std::endl;
}

inline SIGN resultToLType(std::ptrdiff_t *result){
    SIGN ret_type = SIGN::NONE; 
    switch (*result)
    {
    case 0:
        ret_type = SIGN::STOP;
        break;
    case 1:
        ret_type = SIGN::TURN_LEFT;
        break;
    case 2:
        ret_type = SIGN::TURN_RIGHT;
        break;
    case 3:
        ret_type = SIGN::KEEP_FORWARD;
        break;
    case 4:
        ret_type = SIGN::NONE;
        break;
    case 5:
        ret_type = SIGN::GATE1;
        break;
    case 6:
        ret_type = SIGN::GATE2;
        break;
    case 7:
        ret_type = SIGN::GATE3;
        break;
    }
    return ret_type;
}

inline void testModelHandler(const std::string &model_path, const std::string &img_path){
    ModelHandler mHAndler(model_path);
    cv::Mat image = cv::imread(img_path);
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