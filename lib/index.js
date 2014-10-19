
var binding = require('../build/Release/binding');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

var Phidget = function() {
  this.create                 = function()                            { return binding.create(); };
  this.open                   = function(handle, serialNumber)        { return binding.open(handle, serialNumber); };
  this.waitForAttachment      = function(handle, milliseconds)        { return binding.waitForAttachment(handle, milliseconds); };
  this.close                  = function(handle)                      { return binding.close(handle); };
  this.remove                 = function(handle)                      { return binding.remove(handle); };
  this.getDeviceName          = function(handle)                      { return binding.getDeviceName(handle); };
  this.getSerialNumber        = function(handle)                      { return binding.getSerialNumber(handle); };
  this.getDeviceVersion       = function(handle)                      { return binding.getDeviceVersion(handle); };
  this.getDeviceStatus        = function(handle)                      { return binding.getDeviceStatus(handle); };
  this.getLibraryVersion      = function()                            { return binding.getLibraryVersion(); };
  this.getDeviceType          = function(handle)                      { return binding.getDeviceType(handle); };
  this.getInputCount          = function(handle)                      { return binding.getInputCount(handle); };
  this.getBridgeValue         = function(handle, index)               { return binding.getBridgeValue(handle, index); };
  this.getBridgeMax           = function(handle, index)               { return binding.getBridgeMax(handle, index); };
  this.getBridgeMin           = function(handle, index)               { return binding.getBridgeMin(handle, index); };
  this.setEnabled             = function(handle, index, state)        { return binding.setEnabled(handle, index, state); };
  this.getEnabled             = function(handle)                      { return binding.getEnabled(handle); };
  this.getGain                = function(handle, index)               { return binding.getGain(handle, index); };
  this.setGain                = function(handle, index, gain)         { return binding.setGain(handle, index, gain); };
  this.getDataRate            = function(handle)                      { return binding.getDataRate(handle); };
  this.setDataRate            = function(handle, milliseconds)        { return binding.setDataRate(handle, milliseconds); };
  this.getDataRateMax         = function(handle)                      { return binding.getDataRateMax(handle); };
  this.getDataRateMin         = function(handle)                      { return binding.getDataRateMin(handle); };
};

util.inherits(Phidget, EventEmitter);

module.exports = new Phidget();

binding.context.attachHandler       = function(handle) { module.exports.emit("attach", handle); };
binding.context.detachHandler       = function(handle) { module.exports.emit("detach", handle); };
binding.context.errorHandler        = function(handle, errorString) { module.exports.emit("error", handle, errorString); };
binding.context.dataHandler         = function(handle, index, value) { module.exports.emit("data", handle, index, value); };
