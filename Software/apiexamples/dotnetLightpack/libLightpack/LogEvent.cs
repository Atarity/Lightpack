using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace libLightpack
{
    public class LogEventArgs : EventArgs
    {
        private readonly string _message;

        public LogEventArgs(string message): base()
        {
            _message = message;
        }

        public string Message { get { return _message; } }
    }
}
