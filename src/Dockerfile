FROM gcc:14
WORKDIR /app
COPY include ./include
COPY src ./src
RUN g++ -std=c++17 -Iinclude src/main.cpp src/SentinelVault.cpp src/LogWatcher.cpp -o sentinel-vault
CMD ["./sentinel-vault", "--watch"]
