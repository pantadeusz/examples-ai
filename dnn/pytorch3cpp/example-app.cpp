#include <torch/torch.h>

#include "thirdparty/lodepng.h"
// Define a new Module.
struct Net : torch::nn::Module {
    Net() {
        // Construct and register two Linear submodules.
        c2d1 = register_module("c2d1", torch::nn::Conv2d(1, 32, torch::ExpandingArray < 2 > {3, 3}));
        fc1 = register_module("fc1", torch::nn::Linear(26 * 26 * 32, 64));
        fc2 = register_module("fc2", torch::nn::Linear(64, 32));
        fc3 = register_module("fc3", torch::nn::Linear(32, 10));
    }

    // Implement the Net's algorithm.
    torch::Tensor forward(torch::Tensor x) {
        // Use one of many tensor manipulation functions.
        x = torch::relu(c2d1->forward(x));
        x = torch::relu(fc1->forward(x.reshape({x.size(0), 26 * 26 * 32})));
        x = torch::dropout(x, /*p=*/0.1, /*train=*/is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
        return x;
    }

    // Use one of many "standard library" modules.
    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
    torch::nn::Conv2d c2d1{nullptr};
};

std::vector<std::vector<float>> split_vector(std::vector<float> v, int l) {
    std::vector<std::vector<float>> ret;
    std::vector<float> inner;
    for (auto e : v) {
        inner.push_back(e);
        if (inner.size() == l) {
            ret.push_back(inner);
            inner.clear();
        }
    }
    if (inner.size() != 0) throw std::invalid_argument("The input vector cannot be divided by " + std::to_string(l));
    return ret;
}

auto argmax = [](auto v) -> std::size_t{
    return std::max_element(v.begin(), v.end()) - v.begin();
};

template<typename T>
std::vector<T> to_vector(torch::Tensor result) {
    auto sizes = result.sizes(); // Get tensor dimensions
    int size = [&](){int s = 1; for (auto x:sizes) s*=x;return s;}();
    auto ptr = result.to(torch::kCPU).contiguous().data_ptr<T>();
    return std::vector<T>(ptr, ptr+size);
}
int main() {
    torch::Device device(torch::kCPU);
    if (torch::cuda::is_available()) {
        device = torch::Device(torch::kCUDA);
        std::cout << "CUDA is available. Moving model to GPU." << std::endl;
    }
    // Create a new Net.
    auto net = std::make_shared<Net>();
    // Create a multi-threaded data loader for the MNIST dataset.
    auto data_loader = torch::data::make_data_loader(
            torch::data::datasets::MNIST("./data/MNIST/raw").map(
                    torch::data::transforms::Stack<>()),
            /*batch_size=*/4096);

    torch::optim::Adam optimizer(net->parameters(), /*lr=*/0.001);
    try {
        torch::load(net, "net.pt");
    } catch (...) {
        std::cout << "some error lading model" << std::endl;
    }
    for (size_t epoch = 1; epoch <= 10; ++epoch) {
        size_t batch_index = 0;
        // Iterate the data loader to yield batches from the dataset.
        for (auto &batch: *data_loader) {
            auto x_batch = batch.data.data().to(device);
            auto y_batch = batch.target.data().to(device);
            if (batch_index == 0) {
                std::cout << x_batch.sizes() << " " << y_batch.sizes() << std::endl;
            }
            // Reset gradients.
            optimizer.zero_grad();
            net->to(device);
            // Execute the model on the input data.
            torch::Tensor prediction = net->forward(x_batch);
            // Compute a loss value to judge the prediction of our model.
            torch::Tensor loss = torch::nll_loss(prediction, y_batch);
            // Compute gradients of the loss w.r.t. the parameters of our model.
            loss.backward();
            // Update the parameters based on the calculated gradients.
            optimizer.step();
            // Output the loss and checkpoint every 100 batches.
            if (++batch_index % 10 == 0) {
                std::cout << "Epoch: " << epoch << " | Batch: " << batch_index
                          << " | Loss: " << loss.item<float>() << "        \r";
                std::cout.flush();
                // Serialize your model periodically as a checkpoint.
                torch::save(net, "net.pt");
            }
        }
        std::cout << std::endl;
    }
    {
        torch::InferenceMode guard(true);
        for (auto &batch: *data_loader) {
            auto x_batch = batch.data.data().to(device);
            auto y_batch = batch.target.data().to(device);
            net->to(device);
            auto result = net->forward(x_batch);
            std::vector<std::vector<float>> result_vec = split_vector(to_vector<float>(result),10);
            std::vector<std::vector<float>> images_float = split_vector(to_vector<float>(x_batch),28*28);
            std::vector<long> truth_vec = to_vector<long>(y_batch);

            std::vector<int> max_index;
            for(auto& v:result_vec) {
                max_index.push_back(argmax(v));
            }
            for (int i = 0; i < std::min((int)max_index.size(), (int)20); i++) {
                std::cout << max_index[i] << " " << truth_vec[i] << std::endl;
                std::vector<unsigned char> image;
                for (int xx = 0; xx < images_float[i].size(); xx++) {
                    image.push_back(images_float[i][xx]*255.0);
                    image.push_back(images_float[i][xx]*255.0);
                    image.push_back(images_float[i][xx]*255.0);
                    image.push_back(255);
                }
                lodepng::encode("__ret" + std::to_string(i) + "_" + std::to_string(max_index[i]) + ".png", image, 28,28);
            }
            std::cout <<x_batch.sizes() << " " << y_batch.sizes() << "   " << max_index.size() << " " << truth_vec.size() <<  std::endl;
            break;
        }
    }

    return 0;
}
