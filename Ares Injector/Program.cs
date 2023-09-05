using System;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Diagnostics;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Drawing;

public class AresWebData
{
    public bool success { get; set; }
    public JsonElement data { get; set; }
}

public class AresUser
{
    public string uuid { get; set; }
    public string token { get; set; }
    public string email { get; set; }
    public bool premium { get; set; }
    public DateTime premiumExpires { get; set; }
}

namespace Ares_Injector
{
    internal static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {

            AresUtils.Init();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            if (args.Length > 0)
            {
                AresUtils.HandleAresUri(args[0]);

                if (AresSockets.version != null && AresSockets.version.Length > 0)
                {
                    var user = AresSockets.getUser();
                    if(user.uuid.Length <= 0)
                    {
                        Application.Run(new Login());
                    } else
                    {
                        Application.Run(new Inject());
                    }
                } else
                {
                    Application.Run(new Login());
                }
            } else
            {
                Application.Run(new Login());
            };
        }

        internal class AresSockets
        {
            internal static string version { get; set; }
            internal static string token { get; set; }

            internal static TcpClient client { get; set; }
            internal static NetworkStream stream { get; set; }

            internal static void getVersion()
            {
                if ((token == null || token.Length <= 0))
                {
                    MessageBox.Show("Missing token!");
                }
                else
                {
                    try
                    {
                        string ipAddress = "139.162.240.124";
                        int port = 8000;

                        if (client == null)
                            client = new TcpClient(ipAddress, port);
                        if (stream == null)
                            stream = client.GetStream();

                        var json = new
                        {
                            token = token,
                            type = "version"
                        };

                        string jsonData = JsonSerializer.Serialize(json);
                        byte[] data = Encoding.UTF8.GetBytes(jsonData);

                        stream.Write(data, 0, data.Length);

                        byte[] receiveBuffer = new byte[1024];
                        int bytesRead = stream.Read(receiveBuffer, 0, receiveBuffer.Length);
                        string responseData = Encoding.UTF8.GetString(receiveBuffer, 0, bytesRead);

                        var result = JsonSerializer.Deserialize<AresWebData>(responseData);

                        if (result != null)
                        {
                            JsonElement dataElement = result.data;

                            if (dataElement.ValueKind == JsonValueKind.String)
                            {
                                MessageBox.Show(dataElement.GetString());
                            }
                            else if (dataElement.ValueKind == JsonValueKind.Object)
                            {
                                dataElement = dataElement.GetProperty("version");

                                if (dataElement.ValueKind == JsonValueKind.String)
                                {
                                    version = dataElement.GetString();
                                }
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show($"Failed to validate token: {ex.Message}");
                    }
                }
            }
            internal static AresUser getUser()
            {
                var user = new AresUser();

                try
                {
                    if (stream != null)
                    {
                        var json = new
                        {
                            token = token,
                            version = version,
                            type = "login"
                        };

                        string jsonData = JsonSerializer.Serialize(json);
                        byte[] data = Encoding.UTF8.GetBytes(jsonData);

                        stream.Write(data, 0, data.Length);

                        byte[] receiveBuffer = new byte[1024];
                        int bytesRead = stream.Read(receiveBuffer, 0, receiveBuffer.Length);
                        string responseData = Encoding.UTF8.GetString(receiveBuffer, 0, bytesRead);

                        var result = JsonSerializer.Deserialize<AresWebData>(responseData);

                        if (result != null && result.success)
                            user = JsonSerializer.Deserialize<AresUser>(result.data.ToString());
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Failed to get version: {ex.Message}");
                }

                return user;
            }
        }

        internal class AresUtils
        {
            internal static void Init()
            {
                try
                {
                    Process currentProcess = Process.GetCurrentProcess();
                    string currentExecutablePath = currentProcess.MainModule.FileName;

                    var key = Registry.ClassesRoot.CreateSubKey("ares");
                    key.SetValue("", "URL: Ares Protocol");
                    key.SetValue("URL Protocol", "");

                    key.CreateSubKey("shell").CreateSubKey("open").CreateSubKey("command").SetValue("", $"\"{currentExecutablePath}\" \"%1\"");
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error registering custom URI scheme: {ex.Message}");
                }
            }
            internal static void HandleAresUri(string uri)
            {
                try
                {
                    if (uri.StartsWith("ares://token="))
                    {
                        string token = uri.Substring("ares://token=".Length);
                        token = token.TrimEnd('/');

                        AresSockets.token = token;
                        AresSockets.getVersion();
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error handling Ares URI: {ex.Message}");
                }
            }
            internal static void UnregisterProtocol()
            {
                try
                {
                    Registry.ClassesRoot.DeleteSubKeyTree("ares");
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error deleting custom URI: {ex.Message}");
                }
            }
        }
    }
}