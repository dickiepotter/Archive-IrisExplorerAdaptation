using System;
using System.Collections.Generic;
using System.Text;

namespace IrisSharp
{
    public abstract class Connection
    {
    }

    public class InputConnection: Connection
    { 
        private readonly int id;

        public InputConnection(int id)
        {
            this.id = id;
        }
    }

    public class OutputConnection: Connection
    { 
    
    }
}
