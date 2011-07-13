//
// Urho3D Engine
// Copyright (c) 2008-2011 Lasse ��rni
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "Context.h"
#include "File.h"
#include "Image.h"
#include "ProcessUtils.h"

#include <cstdlib>
#include <stb_image.h>
#include <stb_image_write.h>

#include "DebugNew.h"

SharedPtr<Context> context_(new Context());

int main(int argc, char** argv);
void Run(const Vector<String>& arguments);

int main(int argc, char** argv)
{
    Vector<String> arguments;
    
    for (int i = 1; i < argc; ++i)
        arguments.Push(String(argv[i]));
    
    Run(arguments);
    return 0;
}

void Run(const Vector<String>& arguments)
{
    if (arguments.Size() < 1)
    {
        ErrorExit(
            "Usage: NormalMapTool <inputfile>\n\n"
            "Output file will be saved as inputfile.dds\n"
        );
    }
    
    File source(context_);
    source.Open(arguments[0]);
    Image image(context_);
    if (!image.Load(source))
        ErrorExit("Could not load input file " + arguments[0]);
    
    unsigned comp = image.GetComponents();
    if (comp < 3 && comp > 4)
        ErrorExit("Image must contain 3 or 4 components");
    
    SharedArrayPtr<unsigned char> buffer(new unsigned char[image.GetWidth() * image.GetHeight() * 4]);
    unsigned char* srcData = image.GetData();
    unsigned char* destData = buffer.RawPtr();
    
    for (int y = 0; y < image.GetHeight(); ++y)
    {
        for (int x = 0; x < image.GetWidth(); ++x)
        {
            unsigned char r = *srcData++;
            unsigned char g = *srcData++;
            unsigned char b = *srcData++;
            if (comp == 4)
                srcData++;
            
            // Store X as alpha, and Y as color. Z can be reconstructed
            *destData++ = g;
            *destData++ = g;
            *destData++ = g;
            *destData++ = r;
        }
    }
    
    String tempDestName = arguments[0].Split('.')[0] + ".tga";
    stbi_write_tga(tempDestName.CString(), image.GetWidth(), image.GetHeight(), 4, buffer.RawPtr());
    
    String command = "texconv -f DXT5 -ft DDS -if NONE " + tempDestName;
    int ret = system(command.CString());
    
    if (ret)
        ErrorExit("Failed to convert to DXT5");
    
    remove(tempDestName.CString());
}
