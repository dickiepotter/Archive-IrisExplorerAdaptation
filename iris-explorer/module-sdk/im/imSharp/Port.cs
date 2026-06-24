using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;

namespace IrisSharp
{
    public abstract class Port
    {
        protected string name ="";

        public string Name
        {
            get { return name; }
        }

        protected int id =0;

        public int Id
        {
            get { return id; }
        }

        protected string typeName = "";

        public string TypeName
        {
            get { return typeName; }
        }

        public abstract int ConnectionCount{get;}
        public abstract Connection[] Connections { get;}
    }

    public class InputPort : Port 
    {
        public InputPort(string name)
        {
            
            this.name = name;
            id = IrisNative.cxInputPortOpen(name);

            try
            {
                IntPtr ptr = IrisNative.cxInputTypeGet(id);
                typeName = id >= 0 ? Marshal.PtrToStringAnsi(ptr): "Unknown";
            } 
            catch (Exception e) { Console.WriteLine(e.Message); }
            
            Console.WriteLine(ToString());
        }

        public override int ConnectionCount { get { return IrisNative.cxInputConnectsGet(id); } }

        public int[] ConnectionIds
        { 
            get 
            {
                IntPtr ptr = IntPtr.Zero;
                int count = IrisNative.cxInputDataConnIDGet(id, ref ptr);

                int[] connectionIDs = new int[count];
                Marshal.Copy(ptr, connectionIDs, 0, count);

                return connectionIDs;
            }
        }
        
        public override Connection[] Connections 
        {
            get
            {
                int[] ids = ConnectionIds;
                InputConnection[] toRet = new InputConnection[ids.Length];

                for (int i = 0; i < toRet.Length; i++)
                    toRet[i] = new InputConnection(ids[i]);

                return toRet;
            }   
        }

        public override string ToString()
        {
            return String.Format("Input port {0}({1}) containing {2} data with {3} connections.", name, id, typeName, ConnectionCount);
        }
    }

    public class OutputPort : Port
    {
        public OutputPort(string name)
        {
            this.name = name;
            id = IrisNative.cxOutputPortOpen(name);

            try
            {
                IntPtr ptr = IrisNative.cxOutputTypeGet(id);
                typeName = id >= 0 ? Marshal.PtrToStringAnsi(ptr) : "Unknown";
            }
            catch (Exception e) { Console.WriteLine(e.Message); }

            Console.WriteLine(ToString());
        }

        public override int ConnectionCount { get { return IrisNative.cxOutputConnectsGet(id); } }

        public override Connection[] Connections
        {
            get { return null; }
        }

        public override string ToString()
        {
            return String.Format("Output port {0}({1}) containing {2} data with {3} connections.", name, id, typeName, ConnectionCount);
        }
    }
}
