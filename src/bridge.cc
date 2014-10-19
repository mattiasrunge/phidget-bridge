#include <node.h>
#include <v8.h>
#include <phidget21.h>
#include <vector>
#include <string>

using namespace v8;

enum Events
{
    ATTACH,
    DETACH,
    ERROR,
    DATA
};

class Baton
{
public:
    Events event;
    std::string errorString;
    int index;
    double value;
    long handle;
};

static std::vector<Baton*> batons;
Persistent<Object> contextObj;
static uv_async_t async;
static uv_mutex_t mutex;

int CCONV attachHandler(CPhidgetHandle handle, void *userptr)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = ATTACH;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV detachHandler(CPhidgetHandle handle, void *userptr)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = DETACH;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV errorHandler(CPhidgetHandle handle, void *userptr, int errorCode, const char *errorString)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = ERROR;
    baton->errorString = errorString;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV dataHandler(CPhidgetBridgeHandle handle, void *usrptr, int index, double value)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = DATA;
    baton->index = index;
    baton->value = value;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

Handle<Value> create(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;
    CPhidgetHandle handle = 0;

    errorCode = CPhidgetBridge_create((CPhidgetBridgeHandle*)&handle);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_set_OnAttach_Handler(handle, attachHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_set_OnDetach_Handler(handle, detachHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_set_OnError_Handler(handle, errorHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_set_OnBridgeData_Handler((CPhidgetBridgeHandle)handle, dataHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New((long)handle));
}

Handle<Value> open(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or serial number argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or serial number argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_open((CPhidgetHandle)args[0]->IntegerValue(), args[1]->Int32Value());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> waitForAttachment(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or milliseconds argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or milliseconds argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_waitForAttachment((CPhidgetHandle)args[0]->IntegerValue(), args[1]->Int32Value());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> close(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_close((CPhidgetHandle)args[0]->IntegerValue());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> remove(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_delete((CPhidgetHandle)args[0]->IntegerValue());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> getDeviceName(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription, *deviceName;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceName((CPhidgetHandle)args[0]->IntegerValue(), &deviceName);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(String::New(deviceName));
}

Handle<Value> getSerialNumber(const Arguments& args)
{
    HandleScope scope;
    int errorCode, serialNumber;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getSerialNumber((CPhidgetHandle)args[0]->IntegerValue(), &serialNumber);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(serialNumber));
}

Handle<Value> getDeviceVersion(const Arguments& args)
{
    HandleScope scope;
    int errorCode, deviceVersion;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceVersion((CPhidgetHandle)args[0]->IntegerValue(), &deviceVersion);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(deviceVersion));
}

Handle<Value> getDeviceStatus(const Arguments& args)
{
    HandleScope scope;
    int errorCode, deviceStatus;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceStatus((CPhidgetHandle)args[0]->IntegerValue(), &deviceStatus);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(deviceStatus));
}

Handle<Value> getLibraryVersion(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription, *libraryVersion;

    errorCode = CPhidget_getLibraryVersion(&libraryVersion);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(String::New(libraryVersion));
}

Handle<Value> getDeviceType(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription, *deviceType;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceType((CPhidgetHandle)args[0]->IntegerValue(), &deviceType);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(String::New(deviceType));
}

Handle<Value> getInputCount(const Arguments& args)
{
    HandleScope scope;
    int errorCode, count;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getInputCount((CPhidgetBridgeHandle)args[0]->IntegerValue(), &count);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(count));
}

Handle<Value> getBridgeValue(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    double value;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getBridgeValue((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &value);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(value));
}

Handle<Value> getBridgeMax(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    double max;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getBridgeMax((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &max);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(max));
}

Handle<Value> getBridgeMin(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    double min;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getBridgeMin((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &min);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(min));
}

Handle<Value> setEnabled(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 3)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle, index or enabledState argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle, index or enabledState argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_setEnabled((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), args[2]->Int32Value());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> getEnabled(const Arguments& args)
{
    HandleScope scope;
    int errorCode, enabledState;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getEnabled((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &enabledState);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(enabledState));
}

Handle<Value> getGain(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    CPhidgetBridge_Gain gain;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getGain((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &gain);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New((unsigned int)gain));
}

Handle<Value> setGain(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 3)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle, index or gain argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle, index or gain argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_setGain((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value(), (CPhidgetBridge_Gain)args[2]->Int32Value());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> getDataRate(const Arguments& args)
{
    HandleScope scope;
    int errorCode, milliseconds;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getDataRate((CPhidgetBridgeHandle)args[0]->IntegerValue(), &milliseconds);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(milliseconds));
}

Handle<Value> setDataRate(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or milliseconds argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or milliseconds argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_setDataRate((CPhidgetBridgeHandle)args[0]->IntegerValue(), args[1]->Int32Value());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> getDataRateMax(const Arguments& args)
{
    HandleScope scope;
    int errorCode, max;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getDataRateMax((CPhidgetBridgeHandle)args[0]->IntegerValue(), &max);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(max));
}

Handle<Value> getDataRateMin(const Arguments& args)
{
    HandleScope scope;
    int errorCode, min;
    const char *errorDescription;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetBridge_getDataRateMin((CPhidgetBridgeHandle)args[0]->IntegerValue(), &min);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New(min));
}

void eventCallback(uv_async_t *handle, int status /*UNUSED*/)
{
    HandleScope scope;
    uv_mutex_lock(&mutex);

    for (unsigned i = 0; i < batons.size(); i++)
    {
        Baton *baton = batons[i];

        switch (baton->event)
        {
            case ATTACH:
            {
                Local<Value> args[] = { Number::New(baton->handle) };
                node::MakeCallback(contextObj, "attachHandler", 1, args);
                break;
            }
            case DETACH:
            {
                Local<Value> args[] = { Number::New(baton->handle) };
                node::MakeCallback(contextObj, "detachHandler", 1, args);
                break;
            }
            case ERROR:
            {
                Local<Value> args[] = { Number::New(baton->handle), String::New(baton->errorString.c_str()) };
                node::MakeCallback(contextObj, "errorHandler", 2, args);
                break;
            }
            case DATA:
            {
                Local<Value> args[] = { Number::New(baton->handle), Number::New(baton->index), Number::New(baton->value) };
                node::MakeCallback(contextObj, "dataHandler", 3, args);
                break;
            }
        }


        delete baton;
    }

    batons.erase(batons.begin(), batons.end());
    uv_mutex_unlock(&mutex);
}

void init(Handle<Object> target)
{
    contextObj = Persistent<Object>::New(Object::New());

    target->Set(String::New("context"), contextObj);
    target->Set(String::New("create"), FunctionTemplate::New(create)->GetFunction());
    target->Set(String::New("open"), FunctionTemplate::New(open)->GetFunction());
    target->Set(String::New("waitForAttachment"), FunctionTemplate::New(waitForAttachment)->GetFunction());
    target->Set(String::New("close"), FunctionTemplate::New(close)->GetFunction());
    target->Set(String::New("remove"), FunctionTemplate::New(remove)->GetFunction());
    target->Set(String::New("getDeviceName"), FunctionTemplate::New(getDeviceName)->GetFunction());
    target->Set(String::New("getSerialNumber"), FunctionTemplate::New(getSerialNumber)->GetFunction());
    target->Set(String::New("getDeviceVersion"), FunctionTemplate::New(getDeviceVersion)->GetFunction());
    target->Set(String::New("getDeviceStatus"), FunctionTemplate::New(getDeviceStatus)->GetFunction());
    target->Set(String::New("getLibraryVersion"), FunctionTemplate::New(getLibraryVersion)->GetFunction());
    target->Set(String::New("getDeviceType"), FunctionTemplate::New(getDeviceType)->GetFunction());
    target->Set(String::New("getInputCount"), FunctionTemplate::New(getInputCount)->GetFunction());
    target->Set(String::New("getBridgeValue"), FunctionTemplate::New(getBridgeValue)->GetFunction());
    target->Set(String::New("getBridgeMax"), FunctionTemplate::New(getBridgeMax)->GetFunction());
    target->Set(String::New("getBridgeMin"), FunctionTemplate::New(getBridgeMin)->GetFunction());
    target->Set(String::New("setEnabled"), FunctionTemplate::New(setEnabled)->GetFunction());
    target->Set(String::New("getEnabled"), FunctionTemplate::New(getEnabled)->GetFunction());
    target->Set(String::New("getGain"), FunctionTemplate::New(getGain)->GetFunction());
    target->Set(String::New("setGain"), FunctionTemplate::New(setGain)->GetFunction());
    target->Set(String::New("getDataRate"), FunctionTemplate::New(getDataRate)->GetFunction());
    target->Set(String::New("setDataRate"), FunctionTemplate::New(setDataRate)->GetFunction());
    target->Set(String::New("getDataRateMax"), FunctionTemplate::New(getDataRateMax)->GetFunction());
    target->Set(String::New("getDataRateMin"), FunctionTemplate::New(getDataRateMin)->GetFunction());

    uv_async_init(uv_default_loop(), &async, eventCallback);
}

NODE_MODULE(binding, init);
