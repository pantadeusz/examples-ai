import torch
from torch import nn
from torch.utils.data import DataLoader
from torchvision import datasets
from torchvision.transforms import ToTensor
from torchvision.utils import save_image
# Download training data from open datasets.
training_data = datasets.FashionMNIST(
    root="data",
    train=True,
    download=True,
    transform=ToTensor(),
)

# Download test data from open datasets.
test_data = datasets.FashionMNIST(
    root="data",
    train=False,
    download=True,
    transform=ToTensor(),
)

batch_size = 1000

print(training_data[0])
raw_training_data = [ (elem[0],elem[0]) for elem in training_data ]
raw_test_data = [ (elem[0],elem[0]) for elem in test_data ]
# Create data loaders.
train_dataloader = DataLoader(raw_training_data, batch_size=batch_size, shuffle=True)
test_dataloader = DataLoader(raw_test_data, batch_size=batch_size, shuffle=True )

for X, Y in test_dataloader:
    print(f"Shape of X [N, C, H, W]: {X.shape} {X.dtype}")
    print(f"Shape of y: {Y.shape} {Y.dtype}")
    break

# Get cpu, gpu or mps device for training.
device = (
    "cuda"
    if torch.cuda.is_available()
    else "mps"
    if torch.backends.mps.is_available()
    else "cpu"
)
print(f"Using {device} device")

# Define model
class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        
        self.cnn_stack = nn.Sequential(
            nn.Conv2d(1,32,(3,3)), # 26*26*32
            nn.ReLU(),
            nn.MaxPool2d(2), #, stride=2), #  13 * 13* 32
            nn.Conv2d(32,32,(3,3)), # 11 * 11 * 32
            nn.ReLU(),
        )
        self.flatten = nn.Flatten()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(11*11*32, 512),
            #nn.Linear(28*28, 512),
            nn.ReLU(),
            nn.Dropout(0.5),
            nn.Linear(512, 512),
            nn.ReLU(),
            nn.Dropout(0.5),
            nn.Linear(512, 32),
            nn.Softmax(1)
        )
        self.decoder_stack = nn.Sequential(
            nn.Linear(32, 512),
            nn.ReLU(),
            nn.Dropout(0.5),
            nn.Linear(512, 1024),
            nn.ReLU(),
            nn.Linear(1024, 28*28),
            #nn.ReLU(),
            #nn.ReLU(),
            nn.Unflatten(1,(1,28,28))
        )

    def forward(self, x):
        x = self.cnn_stack(x)
        x = self.flatten(x)
        logits = self.linear_relu_stack(x)
        
        x = self.decoder_stack(logits)
        return x

model = NeuralNetwork().to(device)
print(model)

loss_fn = nn.MSELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=3e-4)


def train(dataloader, model, loss_fn, optimizer):
    size = len(dataloader.dataset)
    model.train()
    for batch, (X, y) in enumerate(dataloader):
        X, y = X.to(device), y.to(device)

        # Compute prediction error
        pred = model(X)
        loss = loss_fn(pred, y)

        # Backpropagation
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()

        if batch % 100 == 0:
            loss, current = loss.item(), (batch + 1) * len(X)
            print(f"loss: {loss:>7f}  [{current:>5d}/{size:>5d}]")

def test(dataloader, model, loss_fn):
    size = len(dataloader.dataset)
    num_batches = len(dataloader)
    model.eval()
    test_loss, correct = 0, 0
    with torch.no_grad():
        for X, y in dataloader:
            X, y = X.to(device), y.to(device)
            pred = model(X)
            test_loss += loss_fn(pred, y).item()
    test_loss /= num_batches
    print(f"Test Error: Avg loss: {test_loss:>8f} of {size} items\n")


epochs = 200
for t in range(epochs):
    print(f"Epoch {t+1}\n-------------------------------")
    train(train_dataloader, model, loss_fn, optimizer)
    test(test_dataloader, model, loss_fn)
print("Done!")


torch.save(model.state_dict(), "modelautoenc.pth")
print("Saved PyTorch Model State to modelautoenc.pth")

model = NeuralNetwork().to(device)
model.load_state_dict(torch.load("modelautoenc.pth", weights_only=True))

eval_model = model.eval() # end of training
#test_row = test_data[0]

with torch.no_grad():
    i = 0
    for test_row in test_data:
        i = i + 1
        x, y = test_row[0], test_row[1]
        x = torch.reshape(x, (1,1,28,28))
        x = x.to(device)
        pred = eval_model(x)
        #print(pred)
        save_image(pred,f'results/mnist_{i}_y.png')
        save_image(x,f'results/mnist_{i}_x.png')
        # predicted, actual = classes[pred[0].argmax(0)], classes[y]
        # print(f'Predicted: "{predicted}", Actual: "{actual}"') #  -> "{pred[0]}"


