#include "ContactListener.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	const b2Body* bodyPtrs[] = { contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody() };
	if (bodyPtrs[0]->GetType() == b2BodyType::b2_dynamicBody &&
		bodyPtrs[1]->GetType() == b2BodyType::b2_dynamicBody)
	{
		Box* boxPtrs[] = {
			reinterpret_cast<Box*>(bodyPtrs[0]->GetUserData()),
			reinterpret_cast<Box*>(bodyPtrs[1]->GetUserData())
		};
		auto& tid0 = typeid(boxPtrs[0]->GetColorTrait());
		auto& tid1 = typeid(boxPtrs[1]->GetColorTrait());

		if (boxPtrs[0]->GetColorTrait().GetColor() == Colors::White && tid0 == tid1)
		{
			contactPatternMap.emplace(contact, Pattern::Split);
		}
		else if (boxPtrs[0]->GetColorTrait().GetColor() == Colors::Red && tid0 == tid1)
		{
			contactPatternMap.emplace(contact, Pattern::Remove);
		}

		std::stringstream msg;
		msg << "Collision between " << tid0.name() << " and " << tid1.name() << std::endl;
		OutputDebugStringA(msg.str().c_str());
	}
}