# UnrealDI
[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

Dependency Injection container for Unreal Engine. Container will take care of objects creation and their lifetime.

This implementation works on top of UObjects and UInterfaces.

## Getting started
First, you need to setup your container and register some types into it:
```cpp
// create builder object
FContainerBuilder Builder;

// register type UMyService as implementation of IMyService interface
Builder.RegisterType<UMyService>().As<IMyService>();
Builder.RegisterType<UMyOtherService>().As<IMyOtherService>();

// create container itself
UObjectContainer* Container = Builder.Build();
```

After that you can ask it to resolve some instances:
```cpp
TScriptInterface<IMyService> MyService = Container->Resolve<IMyService>();
TScriptInterface<IMyOtherService> MyService = Container->Resolve<IMyOtherService>();
```

If you need IMyService in some other class, use `InitDependencies`. Container will automatically call this method when it creates the object and provide all requested dependencies

```cpp
UCLASS()
class UMyOtherService : public UObject, public IMyOtherService
{
    GENERATED_BODY()
public:
    void InitDependencies(TScriptInterface<IMyService> InMyService)
    {
        // InMyService is always valid here, no need to check for it
        MyService = InMyService;

        MyService->DoSomething();
    }

private:
    // store reference to interface inside a UPROPERTY to protect it from GC
    UPROPERTY()
    TScriptInterface<IMyService> MyService;
};
```

If you want container to create only one instance of an object and pass it to others, use `WeakSingleInstance` or `SingleInstance` during registration:

```cpp
Builder.RegisterType<UMyService>().As<IMyService>().SingleInstance();
```

## Supported versions
Latest release of UnrealDI requires at least Unreal 5.1. For Unreal version 5.0 and below, please use [Unreal DI v1.5.0](https://github.com/druhasu/UnrealDI/releases/tag/v1.5.0)  
I test this plugin to work with the latest version of Unreal Engine and two versions before it. If you have any issues, please submit them [here](https://github.com/druhasu/UnrealDI/issues/new)

## Repository structure
This repository contains two plugins:
* UnrealDI — contains DI functionality to be used in game.
* UnrealDITests — contains Tests for main plugin and not intended for shipping.

## Installation
1. Download [latest release](https://github.com/druhasu/UnrealDI/releases)
2. Extract archive contents into your project /Plugins/UnrealDI folder
3. Generate project files and build

## Acknowledgment
UnrealDI is heavily inspired by [Hypodermic](https://github.com/ybainier/Hypodermic), but designed with Unreal Engine specifics in mind.
