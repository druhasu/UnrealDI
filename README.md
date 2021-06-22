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

// create container itself
UObjectContainer* Container = Builder.Build();
```

After that you can ask it to resolve some instances:
```cpp
TScriptInterface<IMyService> MyService = Container->Resolve<IMyService>();
```

## Supported versions
UnrealDI was tested to work with Unreal versions 4.25 and 4.26. It should probably work with 4.27 as well, but no testing was done yet.

## Repository structure
This repository contains two plugins:
* UnrealDI — contains DI functionality to be used in game.
* UnrealDITests — contains Tests for main plugin and not intended for shipping.

## Installation
Copy UnrealDI folder into Plugins inside your project.

## Acknowledgment
UnrealDI is heavily inspired by [Hypodermic](https://github.com/ybainier/Hypodermic), but designed with Unreal Engine specifics in mind.
