using System;
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
                "I am a managed app.\n\n" +
                "I am running inside: [" +
                System.Diagnostics.Process.GetCurrentProcess().ProcessName +
                "]\n\n" + (String.IsNullOrEmpty(pwzArgument)
                    ? "I was not given an argument"
                    : "I was given this argument: [" + pwzArgument + "]"));

            return 0;
        }
    }
}
