#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
struct Preset { const char* name; const char* category; std::array<float, 8> p; };
const Preset factory[] = {
 {"Intimate Room","PIANO",{20,3.5f,70,45,100,12000,35,0}}, {"Close Piano","PIANO",{12,1.8f,42,52,130,14500,24,0}},
 {"Warm Room","PIANO",{28,3.2f,58,68,140,9000,31,0}}, {"Bright Room","PIANO",{14,1.7f,45,18,180,18000,27,0}},
 {"Studio Room","PIANO",{8,0.9f,25,38,200,16000,19,0}}, {"Wide Room","PIANO",{32,4.0f,82,34,100,15500,38,0}},
 {"Concert Hall","PIANO",{52,6.8f,96,25,90,16000,42,1}}, {"Dark Hall","PIANO",{48,7.2f,90,80,120,7800,44,1}},
 {"Vocal Plate","VOCAL",{42,2.8f,63,32,250,14000,26,2}}, {"Warm Plate","VOCAL",{30,2.6f,57,55,180,10500,24,2}},
 {"Bright Plate","GUITAR",{24,3.1f,68,12,210,19000,34,2}}, {"Vintage Plate","SPECIAL",{38,4.5f,68,83,240,7000,39,4}},
 {"Chamber","STRINGS",{36,2.3f,55,44,230,13000,30,3}}, {"Large Chamber","STRINGS",{58,5.4f,85,38,120,14500,40,3}},
 {"Ambient Space","SYNTH / PAD",{76,10.0f,100,18,140,17000,60,5}}, {"Dreamy Shimmer","SYNTH / PAD",{65,8.0f,92,50,170,11000,52,5}},
 {"Guitar Room","GUITAR",{15,1.5f,42,35,150,16000,22,0}}, {"Guitar Hall","GUITAR",{44,4.8f,78,28,110,15000,32,1}},
 {"Strings Hall","STRINGS",{50,5.8f,85,50,130,12500,35,1}}, {"Drum Room","DRUMS",{4,0.65f,24,25,280,13000,18,0}},
 {"Drum Ambience","DRUMS",{18,1.3f,40,20,300,16000,25,5}}, {"Synth Pad","SYNTH / PAD",{60,7.5f,95,40,120,12000,48,5}},
 {"Cinematic Long","CINEMATIC",{92,11.5f,100,42,100,12500,63,5}}, {"Ethereal","CINEMATIC",{85,9.0f,98,25,100,18000,58,5}},
 {"Vintage Tape Space","SPECIAL",{38,4.5f,68,83,240,7000,39,4}}, {"Soft Felt","SPECIAL",{25,2.4f,52,72,160,8500,29,4}},
 {"Percussion Room","DRUMS",{5,0.8f,30,36,280,16000,18,0}}, {"Long Cinematic","CINEMATIC",{92,11.5f,100,42,100,12500,63,5}}
};
const juce::StringArray categories {"ALL PRESETS","PIANO","VOCAL","GUITAR","STRINGS","DRUMS","SYNTH / PAD","CINEMATIC","SPECIAL","USER PRESETS"};
const juce::StringArray effects {"ROOM","HALL","PLATE","CHAMBER","VINTAGE","AMBIENT"};
const juce::StringArray labels {"PRE-DELAY","DECAY","SIZE","DAMPING","LOW CUT","HIGH CUT","MIX"};
const juce::Colour bg(0xff090807), copper(0xffd69a5c), ivory(0xfff4e9df), pink(0xffff4ca6), violet(0xffa25cff);

juce::String compactNumber(double value, int decimals)
{
    auto s = juce::String(value, decimals);
    while (s.containsChar('.') && s.endsWith("0")) s = s.dropLastCharacters(1);
    if (s.endsWith(".")) s = s.dropLastCharacters(1);
    return s;
}
}

juce::String FeltRoomEditor::formatValue(int i, double v) const
{
    switch (i) {
        case 0: return compactNumber(v, 1) + " ms";
        case 1: return compactNumber(v, 2) + " s";
        case 2: case 3: case 6: return compactNumber(v, 1) + "%";
        case 4: return juce::String((int) std::round(v)) + " Hz";
        case 5: return v >= 1000.0 ? compactNumber(v / 1000.0, 1) + " kHz" : compactNumber(v, 0) + " Hz";
        default: return compactNumber(v, 2);
    }
}

void FeltRoomEditor::CopperLook::drawComboBox(juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float>(0, 0, (float) w, (float) h).reduced(1.0f);
    g.setColour(juce::Colour(0xff21150e)); g.fillRoundedRectangle(r, 5.0f);
    g.setColour(juce::Colour(0xffb47c49)); g.drawRoundedRectangle(r, 5.0f, 1.0f);
    g.setColour(juce::Colour(0xffe8b67d)); juce::Path p;
    const float x = (float) w - 22.0f, y = (float) h * 0.5f;
    p.startNewSubPath(x - 5, y - 2); p.lineTo(x, y + 3); p.lineTo(x + 5, y - 2);
    g.strokePath(p, juce::PathStrokeType(1.7f));
    juce::ignoreUnused(box);
}

void FeltRoomEditor::CopperLook::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&)
{
    auto bounds = juce::Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(8.0f);
    auto c = bounds.getCentre(); const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.37f;
    const float rr = radius + 5.0f, angle = start + (end-start) * juce::jlimit(0.0f,1.0f,pos);
    g.setColour(juce::Colours::black.withAlpha(0.75f)); g.fillEllipse(c.x-radius-5,c.y-radius-4,(radius+5)*2,(radius+5)*2);
    g.setColour(juce::Colour(0xff4b3c43)); g.drawEllipse(c.x-rr,c.y-rr,rr*2,rr*2,1.2f);
    juce::Path track; track.addCentredArc(c.x,c.y,rr,rr,0,start,end,true);
    g.setColour(juce::Colour(0xff3d3038)); g.strokePath(track,juce::PathStrokeType(4.2f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    const float active = start + (end-start)*juce::jlimit(0.0f,1.0f,pos);
    juce::Path glow; glow.addCentredArc(c.x,c.y,rr,rr,0,start,active,true);
    g.setColour(pink.withAlpha(0.12f)); g.strokePath(glow,juce::PathStrokeType(10.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    constexpr int segs=72;
    for(int i=0;i<segs;++i){float a0=start+(active-start)*(float)i/segs,a1=start+(active-start)*(float)(i+1)/segs;if(a0>=active-0.0001f)break;juce::Path a;a.addCentredArc(c.x,c.y,rr,rr,0,a0,a1,true);g.setColour(pink.interpolatedWith(violet,(float)i/segs));g.strokePath(a,juce::PathStrokeType(4.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));}
    auto face=juce::Rectangle<float>(c.x-radius,c.y-radius,radius*2,radius*2);
    juce::ColourGradient grad(juce::Colour(0xff29252a),face.getX(),face.getY(),juce::Colour(0xff070708),face.getRight(),face.getBottom(),false);g.setGradientFill(grad);g.fillEllipse(face);
    g.setColour(juce::Colour(0xff5b5057));g.drawEllipse(face,1.2f);g.setColour(juce::Colours::white.withAlpha(0.05f));g.drawEllipse(face.reduced(3),1.0f);
    juce::Point<float> tip(c.x+std::sin(angle)*radius*0.72f,c.y-std::cos(angle)*radius*0.72f),base(c.x+std::sin(angle)*radius*0.16f,c.y-std::cos(angle)*radius*0.16f);
    g.setColour(ivory);g.drawLine(base.x,base.y,tip.x,tip.y,2.1f);g.setColour(violet);g.fillEllipse(tip.x-1.5f,tip.y-1.5f,3,3);
}

FeltRoomEditor::FeltRoomEditor(FeltRoomProcessor& p) : AudioProcessorEditor(p), proc(p)
{
    piano = juce::ImageFileFormat::loadFrom(BinaryData::piano_jpg, BinaryData::piano_jpgSize);
    const char* ids[] = {"PREDELAY","DECAY","SIZE","DAMPING","LOWCUT","HIGHCUT","MIX"};
    for (int i=0;i<7;++i) {
        knobs[i]=std::make_unique<juce::Slider>(); auto& k=*knobs[i];
        k.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); k.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0); k.setLookAndFeel(&look);
        k.setColour(juce::Slider::rotarySliderFillColourId,pink); k.onValueChange=[this]{ for(int n=0;n<7;++n) if(valueLabels[n]) valueLabels[n]->setText(formatValue(n,knobs[n]->getValue()),juce::dontSendNotification); };
        addAndMakeVisible(k); attachments[i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.params,ids[i],k);
        valueLabels[i]=std::make_unique<juce::Label>(); valueLabels[i]->setJustificationType(juce::Justification::centred); valueLabels[i]->setColour(juce::Label::textColourId,ivory); valueLabels[i]->setFont(juce::Font(juce::FontOptions(17.0f))); addAndMakeVisible(*valueLabels[i]);
        knobLabels[i]=std::make_unique<juce::Label>(); knobLabels[i]->setText(labels[i],juce::dontSendNotification); knobLabels[i]->setJustificationType(juce::Justification::centred); knobLabels[i]->setColour(juce::Label::textColourId,juce::Colour(0xffe3d1c1)); knobLabels[i]->setFont(juce::Font(juce::FontOptions(14.0f))); addAndMakeVisible(*knobLabels[i]);
    }
    mode.addItemList({"Room","Hall","Plate","Chamber","Vintage","Ambient"},1); mode.setLookAndFeel(&look);
    modeAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(proc.params,"MODE",mode);
    for(int i=0;i<6;++i){effectTabs[i]=std::make_unique<juce::TextButton>();effectTabs[i]->setButtonText(effects[i]);effectTabs[i]->setClickingTogglesState(false);effectTabs[i]->onClick=[this,i]{selectMode(i);};addAndMakeVisible(*effectTabs[i]);}
    for(int i=0;i<categories.size();++i){categoryButtons[i]=std::make_unique<juce::TextButton>();categoryButtons[i]->setButtonText(categories[i]);categoryButtons[i]->setClickingTogglesState(false);categoryButtons[i]->onClick=[this,i]{selectedCategory=categories[i];rebuildPresetList();repaint();};addAndMakeVisible(*categoryButtons[i]);}
    search.setTextToShowWhenEmpty("⌕   Search presets...",juce::Colour(0xffbca99a));search.setColour(juce::TextEditor::backgroundColourId,juce::Colour(0xff1a100b));search.setColour(juce::TextEditor::textColourId,ivory);search.setColour(juce::TextEditor::outlineColourId,juce::Colour(0xffa7754b));search.setColour(juce::TextEditor::focusedOutlineColourId,pink);search.onTextChange=[this]{searchText=search.getText();rebuildPresetList();};addAndMakeVisible(search);
    presetSelector.setLookAndFeel(&look);presetSelector.setTextWhenNothingSelected("Choose a factory preset");for(int i=0;i<(int)std::size(factory);++i)presetSelector.addItem(factory[i].name,i+1);presetSelector.onChange=[this]{auto id=presetSelector.getSelectedId();if(id>0&&id<=(int)std::size(factory))applyFactoryPreset(factory[id-1].name);};addAndMakeVisible(presetSelector);
    auto styleButton=[](juce::TextButton& b){b.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff1b100b));b.setColour(juce::TextButton::buttonOnColourId,juce::Colour(0xff572449));b.setColour(juce::TextButton::textColourOffId,ivory);b.setColour(juce::TextButton::textColourOnId,juce::Colours::white);};
    for(auto* b:{&save,&load,&settings,&random,&previous,&next,&menuButton}){styleButton(*b);addAndMakeVisible(*b);}
    save.onClick=[this]{savePreset();};load.onClick=[this]{loadPreset();};settings.onClick=[this]{settingsOpen=!settingsOpen;juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,"FELTROOM settings","FELTROOM Reverb\nD.A Production\n\nAudio parameters are controlled by the host.");};
    random.onClick=[this]{auto idx=juce::Random::getSystemRandom().nextInt((int)std::size(factory));applyFactoryPreset(factory[idx].name);};
    previous.onClick=[this]{auto id=presetSelector.getSelectedId();id=id<=1?(int)std::size(factory):id-1;presetSelector.setSelectedId(id);};next.onClick=[this]{auto id=presetSelector.getSelectedId();id=id>=(int)std::size(factory)?1:id+1;presetSelector.setSelectedId(id);};
    menuButton.onClick=[this]{selectedCategory="ALL PRESETS";rebuildPresetList();};
    rebuildPresetList();setSize(1440,1050);setResizable(true,true);setResizeLimits(1000,760,1900,1300);
}
FeltRoomEditor::~FeltRoomEditor(){for(auto& k:knobs)k->setLookAndFeel(nullptr);mode.setLookAndFeel(nullptr);presetSelector.setLookAndFeel(nullptr);}

void FeltRoomEditor::selectMode(int index){mode.setSelectedId(index+1,juce::sendNotificationSync);for(int i=0;i<6;++i)effectTabs[i]->setToggleState(i==index,juce::dontSendNotification);repaint();}
void FeltRoomEditor::applyFactoryPreset(const juce::String& name){for(const auto& p:factory)if(name==p.name){selectedPreset=name;proc.applyPreset(p.p);mode.setSelectedId((int)p.p[7]+1,juce::sendNotificationSync);for(int i=0;i<6;++i)effectTabs[i]->setToggleState(i==(int)p.p[7],juce::dontSendNotification);rebuildPresetList();return;}}
void FeltRoomEditor::rebuildPresetList(){
    for(auto& b:presetButtons)removeChildComponent(b.get());presetButtons.clear();
    std::vector<const Preset*> filtered;
    for(const auto& p:factory){bool cat=selectedCategory=="ALL PRESETS"||selectedCategory=="USER PRESETS"||selectedCategory==p.category;bool query=juce::String(p.name).containsIgnoreCase(searchText);if(cat&&query)filtered.push_back(&p);}
    for(size_t i=0;i<filtered.size();++i){auto* p=filtered[i];auto b=std::make_unique<juce::TextButton>();b->setButtonText(juce::String(p->name)+(selectedPreset==p->name?"   ★":"   ☆"));b->setColour(juce::TextButton::buttonColourId,selectedPreset==p->name?juce::Colour(0xff61351d):juce::Colour(0x00101010));b->setColour(juce::TextButton::buttonOnColourId,juce::Colour(0xff61351d));b->setColour(juce::TextButton::textColourOffId,ivory);b->setColour(juce::TextButton::textColourOnId,juce::Colours::white);b->onClick=[this,name=juce::String(p->name)]{selectedPreset=name;for(int i=0;i<presetSelector.getNumItems();++i)if(presetSelector.getItemText(i)==name){presetSelector.setSelectedItemIndex(i,juce::dontSendNotification);break;}applyFactoryPreset(name);};addAndMakeVisible(*b);presetButtons.push_back(std::move(b));}
    resized();repaint();
}
void FeltRoomEditor::savePreset(){chooser=std::make_unique<juce::FileChooser>("Save FeltRoom preset",juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("FeltRoom-Preset.xml"),"*.xml");chooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting,[this](const juce::FileChooser& fc){auto f=fc.getResult();if(f==juce::File{})return;if(auto xml=proc.params.copyState().createXml())xml->writeTo(f.withFileExtension("xml"));});}
void FeltRoomEditor::loadPreset(){chooser=std::make_unique<juce::FileChooser>("Load FeltRoom preset",juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),"*.xml");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[this](const juce::FileChooser& fc){auto f=fc.getResult();if(f==juce::File{})return;auto xml=juce::XmlDocument::parse(f);if(xml&&xml->hasTagName(proc.params.state.getType()))proc.params.replaceState(juce::ValueTree::fromXml(*xml));});}

void FeltRoomEditor::paint(juce::Graphics& g)
{
    auto b=getLocalBounds().toFloat();g.fillAll(bg);
    // Main piano photograph with cinematic warm tint.
    auto photo=juce::Rectangle<float>(0,0,(float)getWidth(),getHeight()*0.43f);
    if(piano.isValid())g.drawImage(piano,photo,juce::RectanglePlacement::fillDestination);
    juce::ColourGradient dark(juce::Colour(0xd9000000),0,0,juce::Colour(0x22000000),0,photo.getBottom(),false);g.setGradientFill(dark);g.fillRect(photo);
    g.setColour(juce::Colour(0xfff5e9df));g.setFont(juce::Font(juce::FontOptions(44.0f)));g.drawFittedText("F E L T R O O M",0,32,getWidth(),65,juce::Justification::centred,1);
    g.setFont(juce::Font(juce::FontOptions(13.0f)));g.setColour(juce::Colour(0xffe4cdb8));g.drawFittedText("R E V E R B",0,91,getWidth(),28,juce::Justification::centred,1);
    g.setFont(juce::Font(juce::FontOptions(18.0f)));g.setColour(ivory);g.drawText("D.A Production",28,28,250,38,juce::Justification::left);
    // Header action buttons and framing.
    g.setColour(juce::Colour(0xffb67a48));g.drawRoundedRectangle(b.reduced(8),15,1.0f);
    // Glass panel behind controls.
    auto panel=juce::Rectangle<float>(0,getHeight()*0.405f,(float)getWidth(),getHeight()*0.595f);g.setColour(juce::Colour(0xf5080809));g.fillRect(panel);
    g.setColour(juce::Colour(0xff493329));g.drawHorizontalLine((int)(getHeight()*0.685f),24.0f,(float)getWidth()-24.0f);
    // Warm border around preset browser.
    auto presetArea=juce::Rectangle<float>(12,getHeight()*0.692f,(float)getWidth()-24,getHeight()*0.295f);
    juce::ColourGradient edge(juce::Colour(0xffa56c3f),presetArea.getX(),presetArea.getY(),juce::Colour(0xff3a2114),presetArea.getRight(),presetArea.getBottom(),false);g.setGradientFill(edge);g.drawRoundedRectangle(presetArea,13,1.5f);
    g.setColour(juce::Colour(0xffbd8655));g.setFont(juce::Font(juce::FontOptions(14.0f)));g.drawText("D.A PRODUCTION  /  FELT REVERB",28,getHeight()-24,360,15,juce::Justification::left);
}

void FeltRoomEditor::resized()
{
    const int w=getWidth(),h=getHeight();
    const int pad=24, topH=(int)(h*0.405f), tabY=(int)(h*0.415f), tabH=(int)(h*0.075f);
    save.setBounds(w-365,28,112,52);load.setBounds(w-248,28,112,52);settings.setBounds(w-128,28,56,52);
    const int tabGap=8, tabW=(w-2*pad-5*tabGap)/6;
    for(int i=0;i<6;++i){auto& t=*effectTabs[i];t.setBounds(pad+i*(tabW+tabGap),tabY,tabW,tabH);t.setColour(juce::TextButton::buttonColourId,i==0?juce::Colour(0xff572448):juce::Colour(0xe50a0a0c));t.setColour(juce::TextButton::buttonOnColourId,juce::Colour(0xff612348));t.setColour(juce::TextButton::textColourOffId,ivory);t.setColour(juce::TextButton::textColourOnId,juce::Colours::white);t.setLookAndFeel(&look);}
    const int knobY=(int)(h*0.49f), knobH=(int)(h*0.145f), knobW=juce::jmin(132,(w-2*pad)/7);
    const int actualW=(w-2*pad)/7;
    for(int i=0;i<7;++i){int x=pad+i*actualW+(actualW-knobW)/2;knobs[i]->setBounds(x,knobY,knobW,knobH);valueLabels[i]->setBounds(x-8,knobY+knobH-2,knobW+16,25);knobLabels[i]->setBounds(x-8,knobY+knobH+23,knobW+16,22);}
    const int browserY=(int)(h*0.70f), browserBottom=h-32, browserH=browserBottom-browserY;
    menuButton.setBounds(28,browserY+8,180,38);presetSelector.setBounds(290,browserY+8,410,38);previous.setBounds(714,browserY+8,34,38);next.setBounds(750,browserY+8,34,38);search.setBounds(800,browserY+8,w-1010,38);random.setBounds(w-188,browserY+8,158,38);
    const int catsX=28,catsW=260,listY=browserY+56,listH=browserH-64;const int rowH=juce::jmax(23,listH/10);
    for(int i=0;i<categories.size();++i){auto& c=*categoryButtons[i];c.setBounds(catsX,listY+i*rowH,catsW,rowH);c.setColour(juce::TextButton::buttonColourId,selectedCategory==categories[i]?juce::Colour(0xff77411f):juce::Colour(0x00000000));c.setColour(juce::TextButton::textColourOffId,juce::Colour(0xfff0d8c0));}
    const int listX=300,listW=w-listX-30,colGap=12,colW=(listW-2*colGap)/3;const int rowsPerColumn=juce::jmax(1,(int)((presetButtons.size()+2)/3));const int presetRowH=juce::jmax(22,listH/rowsPerColumn);
    for(size_t i=0;i<presetButtons.size();++i){int col=(int)(i/(size_t)rowsPerColumn);int row=(int)(i%(size_t)rowsPerColumn);if(col>2)col=2;int x=listX+col*(colW+colGap);presetButtons[i]->setBounds(x,listY+row*presetRowH,colW,presetRowH);}
    // Mode combo is attached to the host parameter but intentionally hidden; the large tabs are the visible controller.
    mode.setBounds(-100,-100,1,1);
}
