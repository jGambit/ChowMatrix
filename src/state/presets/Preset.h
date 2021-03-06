#pragma once

#include <pch.h>

struct Preset
{
    String name;
    std::unique_ptr<XmlElement> state;
    int index;

    Preset (String presetFile)
    {
        // load xml text from BinaryData
        String xmlText;
        for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        {
            if (String (BinaryData::originalFilenames[i]) == presetFile)
            {
                int dummy = 0;
                xmlText = String (BinaryData::getNamedResource (BinaryData::namedResourceList[i], dummy));
            }
        }

        jassert (xmlText.isNotEmpty()); // preset does not exist!!
        state = XmlDocument::parse (xmlText);
        name = state->getStringAttribute ("name");

        if (auto vtsXml = state->getChildByName ("state"))
        {
            if (auto paramsXml = vtsXml->getChildByName ("Parameters"))
            {
                if (auto presetParamXml = paramsXml->getChildByAttribute ("id", "preset"))
                    index = presetParamXml->getIntAttribute ("value");
            }
        }
    }

    Preset (const File& presetFile)
    {
        state = XmlDocument::parse (presetFile);
        name = state->getStringAttribute ("name");
    }
};
