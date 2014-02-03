using System;
using System.Diagnostics;
using System.Windows.Forms;

namespace CodeInject
{
    public static class SomeClass
    {
        /// <summary>
        /// Somes the method.
        /// </summary>
        /// <param name="pwzArgument">The argument.</param>
        /// <returns>System.Int32.</returns>
        /// <remarks>http://www.codingthewheel.com/archives/how-to-inject-a-managed-assembly-dll/</remarks>
        public static int SomeMethod(String pwzArgument)
        {
            MessageBox.Show(
                "Managed application running inside: [" +
                Process.GetCurrentProcess().ProcessName +
                "]" + Environment.NewLine + (String.IsNullOrEmpty(pwzArgument)
                    ? "Not given an argument"
                    : "Given the argument: [" + pwzArgument + "]"));
            
            return 0;
        }
    }
}
