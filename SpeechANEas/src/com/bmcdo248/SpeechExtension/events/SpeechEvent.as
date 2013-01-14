package com.bmcdo248.SpeechExtension.events
{
	import flash.events.Event;
	
	public class SpeechEvent extends Event
	{
		public static const DATA:String = "Data Recieved";
		
		public var data:String;
		
		public function SpeechEvent(type:String, data:String, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			
			this.data = data;
		}
	}
}