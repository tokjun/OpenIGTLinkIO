#ifndef IGTLIOSESSION_H
#define IGTLIOSESSION_H

#include "igtlioUtilities.h"
#include "igtlioDevice.h"
#include "igtlioCommandDevice.h"

#include "igtlioLogicExport.h"

class vtkMatrix4x4;
class vtkImageData;

typedef vtkSmartPointer<class igtlioSession> igtlioSessionPointer;
typedef vtkSmartPointer<class igtlioConnector> igtlioConnectorPointer;
typedef vtkSmartPointer<class igtlioImageDevice> igtlioImageDevicePointer;
typedef vtkSmartPointer<class igtlioTransformDevice> igtlioTransformDevicePointer;
typedef vtkSmartPointer<class igtlioVideoDevice> igtlioVideoDevicePointer;
typedef vtkSmartPointer<class igtlioStringDevice> igtlioStringDevicePointer;
typedef vtkSmartPointer<class igtlioStatusDevice> igtlioStatusDevicePointer;

/// Convenience interface for a single IGTL connection.
///
/// Example: Send a command to a server and get a response.
///
///     vtkIGTLIOLogicPointer root = vtkIGTLIOLogicPointer::New();
///     vtkIGTLIOSessionPointer session = root->ConnectToServer(example.org, 18333);
///
///     CommandDevicePointer command;
///     command = session->SendCommandQuery("device_id",
///                                         "GetDeviceParameters",
///                                         "<Command><Parameter Name=\"Depth\"/></Command>",
///                                         IGTLIO_BLOCKING, 5);
///
///     STATUS status = command->GetStatus();
///     std::string response = command->GetResponse(); // empty on failure.
///
///
///
class OPENIGTLINKIO_LOGIC_EXPORT igtlioSession : public vtkObject
{
public:
  ///  Send the given command from the given device.
  /// - If using BLOCKING, the call blocks until a response appears or timeout. Return response.
  /// - If using ASYNCHRONOUS, wait for the CommandResponseReceivedEvent event. Return device.
  ///
  igtlioCommandDevicePointer SendCommand(std::string device_id, std::string command, std::string content, IGTLIO_SYNCHRONIZATION_TYPE synchronized = IGTLIO_BLOCKING, double timeout_s = 5);

  ///  Send a command response from the given device. Asynchronous.
  /// Precondition: The given device has received a query that is not yet responded to.
  /// Return device.
  igtlioCommandDevicePointer SendCommandResponse(std::string device_id, std::string command, std::string content);

  ///  Send the given image from the given device. Asynchronous.
  igtlioImageDevicePointer SendImage(std::string device_id,
                                        vtkSmartPointer<vtkImageData> image,
                                        vtkSmartPointer<vtkMatrix4x4> transform);

  ///
  ///  Send the given video frame from the given device. Asynchronous.
#if defined(OpenIGTLink_USE_H264) || defined(OpenIGTLink_USE_VP9) || (defined(OpenIGTLink_USE_X265) && defined(OpenIGTLink_USE_OpenHEVC))
  igtlioVideoDevicePointer SendFrame(std::string device_id,
                               vtkSmartPointer<vtkImageData> image);
#endif

  /// Send the given image from the given device. Asynchronous.
  igtlioTransformDevicePointer SendTransform(std::string device_id, vtkSmartPointer<vtkMatrix4x4> transform);

  /// Send the given string from the given device. Asynchronous.
  igtlioStringDevicePointer SendString(std::string device_id, std::string content);

  /// Send the given status from the given device. Asynchronous.
  igtlioStatusDevicePointer SendStatus(std::string device_id, int code, int subcode, std::string statusstring, std::string errorname);


public:
  static igtlioSession *New();
  vtkTypeMacro(igtlioSession, vtkObject);
  void PrintSelf(ostream&, vtkIndent) VTK_OVERRIDE;

  void StartServer(int serverPort=-1, IGTLIO_SYNCHRONIZATION_TYPE sync=IGTLIO_BLOCKING, double timeout_s=5);
  void ConnectToServer(std::string serverHost, int serverPort=-1, IGTLIO_SYNCHRONIZATION_TYPE sync=IGTLIO_BLOCKING, double timeout_s=5);

  /// Get the underlying Connector object.
  igtlioConnectorPointer GetConnector();
  void SetConnector(igtlioConnectorPointer connector);

private:
  igtlioSession();

  igtlioConnectorPointer Connector;

//  vtkIGTLIOLogicPointer remoteLogic = vtkIGTLIOLogicPointer::New();
//   vtkIGTLIOConnector connector = remoteLogic->Connect(serverHost, serverPort, CLIENT, 5.0 /* timeout in sec*/);
//   if (connector.isNull())
//   {
//     LOG_ERROR("Failed to start PlusServer");
//     exit(EXIT_FAILURE);
//   }

//   vtkIGTLIOCommandPointer command = remoteConnector->SendCommandQuery("PlusServerLauncher", "<Command ... />", SYNCHRONOUS, 5.0 /* timeout in sec*/);
//   std::string response = command.GetResponse();

//   vtkIGTLIOCommandPointer command = remoteLogic->SendCommandQuery("PlusServerLauncher", "<Command ... />", SYNCHRONOUS, 5.0 /* timeout in sec*/, connector);
//   std::string response = command.GetResponse();

//   vtkIGTLIOCommandPointer command = remoteConnector->SendCommandQuery("PlusServerLauncher", "<Command ... />", ASYNCHRONOUS, 5.0 /* timeout in sec*/);
//   // do some work, may check command.GetStatus() ...
//   std::string response = command.GetResponse();

  bool waitForConnection(double timeout_s);
};

#endif // IGTLIOSESSION_H
