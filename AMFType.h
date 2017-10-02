#pragma once
namespace AMF
{
	class AMF0Type
	{
	public:
		AMF0Type(void);
	public:
		~AMF0Type(void);

		enum
		{
			Number = 0,
			Boolean = 1,
			String = 2,
			Object = 3,
			MovieClip = 4,
			Null = 5,
			Undefined = 6,
			Reference = 7,
			MixedArray = 8,
			EndOfObject = 9,
			Array = 10,
			Date = 11,
			LongString = 12,
			ASObject = 13,
			Recordset = 14,
			Xml = 15,
			CustomClass = 16,
			AMF3Data = 17,
		};
	};

	class AMF3Type
	{
	public:
		AMF3Type(void);
	public:
		~AMF3Type(void);

		enum
		{
			Undefined = 0,
			Null = 1,//EndOfObject
			False = 2,
			True = 3,
			Integer = 4,
			Number = 5,
			String = 6,
			XML = 7,
			Date = 8,
			Array = 9,
			Object = 10,
			AvmPlusXml = 11,
			ByteArray = 12,

			Dictionary = 17,
		};
	};
}
