package com.bmcdo248.SpeechExtension.speechDiplomat
{
	import com.bmcdo248.SpeechExtension.events.SpeechEvent;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	
	public class SpeechDiplomat extends EventDispatcher
	{
		private static var context:ExtensionContext = null;
		private static var contextCount:int = 0;
		
		private var running:Boolean = false;
		
		public function SpeechDiplomat()
		{
			super();
			
			if(context == null)
			{
				contextCount ++;
				context = ExtensionContext.createExtensionContext("com.bmcdo248.SpeechExtension", "context" + contextCount);
			}
			
			context.removeEventListener(StatusEvent.STATUS, handleEvent);
			context.addEventListener(StatusEvent.STATUS, handleEvent);
		}
		
		public function dispose():void
		{
			context.dispose();
			context = null;
			contextCount--;
			running = false;
		}
		
		public function isSupported():Boolean
		{
			return context.call("isSupported");
		}
		
		public function startSpeech():Boolean
		{
			if(!running)
			{
				running = context.call("startSpeech");
				return running;
			}
			else
			{
				return true;
			}
		}
		
		private function handleEvent(e:StatusEvent):void
		{
			dispatchEvent(new SpeechEvent(SpeechEvent.DATA, e.code, false, false));
		}
	}
	
}