#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include "battery.h"
#include "settings.h"
#include <iostream>

int main(int argc, char** argv)
{
    // Use the same config file as the GUI application.
    // This is used by QSettings()
    qputenv("XDG_CONFIG_HOME", "/home/nemo/.config");

    const char* logEnvVar = "QT_LOGGING_RULES";
    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i],"-v")) {
            printf("%s %s\n", APP_NAME, APP_VERSION);
            return EXIT_SUCCESS;
        }
        else if(!strcmp(argv[i],"--verbose"))
            qputenv(logEnvVar, "*.info=true;*.debug=false");
        else if(!strcmp(argv[i],"--debug"))
            qputenv(logEnvVar, "*.info=true");
        else if(!strcmp(argv[i],"--help")) {
            printf("%s %s\n\n", APP_NAME, APP_VERSION);
            printf("This binary is meant to run as a service with root access,\n");
            printf("but it can be run manually for debugging purposes, too.\n\n");
            printf("Usage:\n");
            printf("  --verbose     Enable informational messages\n");
            printf("  --debug       Enable informational and debugging messages\n");
            printf("  --help        Print version string and exit\n");
            return EXIT_SUCCESS;
        }
    }
    if(!qEnvironmentVariableIsSet(logEnvVar))
        qputenv(logEnvVar, "*.info=false;*.debug=false");

    QCoreApplication app(argc, argv);

    Settings* settings = new Settings();
    Battery* battery = new Battery(settings);

    QTimer* updater = new QTimer();
    QObject::connect(updater, SIGNAL(timeout()), battery, SLOT(updateData()));
    updater->start(3000);

    int retval = app.exec();

    return retval;
}
