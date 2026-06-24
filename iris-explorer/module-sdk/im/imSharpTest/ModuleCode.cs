using System;
using IrisSharp;

namespace IrisSharpTest
{
    public class ModuleCode: Module
    {
        public override long UserFunction()
        {
            //Console.WriteLine("UserFunction: " + Ports.ToString());
            return 0;
        }

        public override void Init()
        {
            
        }

        public override void Remove()
        {
        }

        public override void ConnectIn(string portName, int id)
        {
            
        }

        public override void DisconnectIn(string portName, int id)
        {
        }

        public override void ConnectOut(string portName, int id)
        {
        }

        public override void DisconnectOut(string portName, int id)
        {
        }

        public override void Create()
        {
            
        }
    }
}
