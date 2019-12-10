# agl-service-optee-hello-world

This git repository contain an AGL service that is calling for an application in the TrustZone.

Because of the nature of the optee_os, it's only possible to test this application on arm based boards.

To work properly, this application also need access to the "tee0" device, a udev rule can be use to grant this access.

Also, you'll need to be sure of the proper deployment of the optee_os on your board using an AGL image.

