using System;
using System.Runtime.InteropServices;
using System.Reflection;

namespace IrisSharp
{
    public abstract class Module
    {
        public string Name
        {
            get { return Marshal.PtrToStringAnsi(IrisNative.cxModuleNameGet()); }
        }

        public string InstanceName
        {
            get { return Marshal.PtrToStringAnsi(IrisNative.cxModuleInstanceNameGet()); }
        }

        public string BaseName
        {
            get { return Marshal.PtrToStringAnsi(IrisNative.cxModuleBaseNameGet()); }
        } 

        public InputPort[] InputPorts
        {
            get
            {
                string[] portNames = InputPortNames;
                InputPort[] toRet = new InputPort[InputPortCount];
                for (int i = 0; i < toRet.Length; i++)
                    toRet[i] = new InputPort(portNames[i]);
                return toRet;
            }
        }

        public OutputPort[] OutputPorts
        {
            get
            {
                string[] portNames = OutputPortNames;
                OutputPort[] toRet = new OutputPort[OutputPortCount];
                for (int i = 0; i < toRet.Length; i++)
                    toRet[i] = new OutputPort(portNames[i]);
                return toRet;
            }
        }

        public int InputPortCount
        {
            get { return IrisNative.cxInputPortCount(); }
        }

        public int OutputPortCount
        {
            get { return IrisNative.cxOutputPortCount(); }
        }

        public string[] InputPortNames
        {
            get
            {
                int count = InputPortCount;
                IntPtr start = IrisNative.cxInputPortNames();
                IntPtr[] ptrArr = new IntPtr[count];

                Marshal.Copy(start, ptrArr, 0, count);

                string[] toRet = new string[count];
                for (int i = 0; i < count; i++)
                    toRet[i] = Marshal.PtrToStringAnsi(ptrArr[i]);

                return toRet;
            }
        }

        public string[] OutputPortNames
        {
            get
            {
                int count = OutputPortCount;
                IntPtr start = IrisNative.cxOutputPortNames();
                IntPtr[] ptrArr = new IntPtr[count];

                Marshal.Copy(start, ptrArr, 0, count);

                string[] toRet = new string[count];
                for (int i = 0; i < count; i++)
                    toRet[i] = Marshal.PtrToStringAnsi(ptrArr[i]);

                return toRet;
            }
        }

        #region Abstract members

        public abstract long UserFunction();
        public abstract void Init();
        public abstract void Remove();
        public abstract void ConnectIn(string portName, int id);
        public abstract void DisconnectIn(string portName, int id);
        public abstract void ConnectOut(string portName, int id);
        public abstract void DisconnectOut(string portName, int id);
        public abstract void Create();
        //public abstract ErrorCode PortCheckIn ( int linkC, IntPtr[] links, ref long portID, long linkIDs );
        //public abstract ErrorCode PortCheckOut ( int linkC, IntPtr[] links, ref long portID );
        
        #endregion

        #region Static Concrete module finder methods

        private static Module LoadModule(string assemblyPath)
        {
            Assembly ass = Assembly.LoadFile(assemblyPath);
            foreach (Type t in ass.GetTypes())
            { 
                Type baseType = t.BaseType;
                while (baseType != null && baseType != typeof(object))
                {
                    if (baseType == typeof(Module))
                        return (Module)t.GetConstructor(Type.EmptyTypes).Invoke(null);
                    baseType = baseType.BaseType;
                }
            }

            throw new ArgumentException("Invalid path to assembly containing a concrete module: " + assemblyPath);
        }

        private static Module LoadModule()
        {
            string name = Marshal.PtrToStringAnsi(IrisNative.cxModuleNameGet());
            return LoadModule(name + ".dll");
        }

        private static Module concreteModule = null;
        public static Module Instance 
        { 
            get 
            {
                if (concreteModule == null)
                    concreteModule = LoadModule();
                return concreteModule;
            }
        }

        #endregion
    }
}
