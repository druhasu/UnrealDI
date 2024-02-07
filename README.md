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
Latest release of UnrealDI requires Unreal 5.1 and was tested to work with Unreal versions 5.1, 5.2, 5.3. 
For Unreal version 5.0 and below, please use [Unreal DI v1.5.0](https://github.com/druhasu/UnrealDI/releases/tag/v1.5.0)

## Repository structure
This repository contains two plugins:
* UnrealDI — contains DI functionality to be used in game.
* UnrealDITests — contains Tests for main plugin and not intended for shipping.

## Installation
1. Download [latest release](https://github.com/druhasu/UnrealDI/releases)
2. Copy UnrealDI folder into Plugins inside your project.

## Acknowledgment
UnrealDI is heavily inspired by [Hypodermic](https://github.com/ybainier/Hypodermic), but designed with Unreal Engine specifics in mind.
