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
