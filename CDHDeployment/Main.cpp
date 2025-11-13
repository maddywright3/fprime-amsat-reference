// ======================================================================
// \title  Main.cpp
// \brief main program for the F' application. Intended for CLI-based systems (Linux, macOS)
//
// ======================================================================
// Used to access topology functions
#include <CDHDeployment/Top/CDHDeploymentTopology.hpp>
#include <CDHDeployment/Top/CDHDeploymentTopologyAc.hpp>
// OSAL initialization
#include <Os/Os.hpp>
// Used for signal handling shutdown
#include <signal.h>
// Used for command line argument processing
#include <getopt.h>
// Used for printf functions
#include <cstdlib>

/**
 * \brief print command line help message
 */
void print_usage(const char* app) {
    (void)printf("Usage: ./%s [options]\n-a\thostname/IP address\n-p\tport_number\n", app);
}

/**
 * \brief shutdown topology cycling on signal
 */
static void signalHandler(int signum) {
    CDHDeployment::stopSimulatedCycle();
}

/**
 * \brief execute the program
 */
int main(int argc, char* argv[]) {
    I32 option = 0;
    CHAR* hostname = nullptr;
    U16 port_number = 0;

    Os::init();

    // Loop while reading the getopt supplied options
    while ((option = getopt(argc, argv, "hp:a:")) != -1) {
        switch (option) {
            case 'a':
                hostname = optarg;
                break;
            case 'p':
                port_number = static_cast<U16>(atoi(optarg));
                break;
            case 'h':
            case '?':
            default:
                print_usage(argv[0]);
                return (option == 'h') ? 0 : 1;
        }
    }

    // Object for communicating state to the topology
    CDHDeployment::TopologyState inputs;
    inputs.hostname = hostname;
    inputs.port = port_number;

    // Setup program shutdown via Ctrl-C
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    (void)printf("Hit Ctrl-C to quit\n");

    // Setup topology
    CDHDeployment::setupTopology(inputs);

    // Start simulated cycle at 1Hz
    CDHDeployment::startSimulatedCycle(Fw::TimeInterval(1, 0));

    // Teardown topology
    CDHDeployment::teardownTopology(inputs);

    (void)printf("Exiting...\n");
    return 0;
}