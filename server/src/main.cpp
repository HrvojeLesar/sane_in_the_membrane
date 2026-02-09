#include "StartupOrchestrator.cpp"

int main(int argc, char* argv[]) {
    sane_in_the_membrane::startup::CStartupOrchestrator start{};
    start.wait();

    return 0;
}
