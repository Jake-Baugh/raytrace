struct Ray
{
	float4 origin;
	float4 direction;
	float4 color;
	bool lastWasHit; // If prev jump missed everything. Next jump will aswell.
};