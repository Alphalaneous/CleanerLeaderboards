#include <Geode/Geode.hpp>
#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/CustomListView.hpp>

using namespace geode::prelude;

class MoreLeaderboards : public CCNode {};

bool shouldExclude() {
	// GDUtils
	if (CCScene::get()->getChildByType<MoreLeaderboards>(0)) return true;
	return false;
}

class $modify(MyGJScoreCell, GJScoreCell) {

	static void onModify(auto& self) {
		(void) self.setHookPriorityPost("GJScoreCell::loadFromScore", Priority::VeryLate);
	}

	CCNode* createStatContainer(ZStringView ID) {
		auto container = CCNode::create();

		container->setLayout(SimpleRowLayout::create()
			->setGap(5.f)
			->setMainAxisScaling(AxisScaling::None)
			->setCrossAxisScaling(AxisScaling::None)
			->setMainAxisAlignment(MainAxisAlignment::Start)
		);

		container->setContentSize({70, m_height / 2 - 5});

		CCNode* parent = m_mainLayer;

		Ref<CCNode> label = parent->getChildByID(fmt::format("{}-label", ID));
		if (!label) {
			parent = m_mainLayer->getChildByID("stats-menu");
			if (!parent) return container;
			label = parent->getChildByID(fmt::format("{}-label", ID));
		}
		Ref<CCNode> icon = parent->getChildByID(fmt::format("{}-icon", ID));

		if (!label || !icon) return container;

		label->removeFromParent();
		icon->removeFromParent();

		auto iconContainer = CCNode::create();
		iconContainer->setID(fmt::format("{}-icon-container", ID));
		iconContainer->setContentSize({20, 20});
		iconContainer->addChild(icon);

		icon->setPosition(iconContainer->getContentSize() / 2);

		static_cast<CCLabelBMFont*>(label.data())->limitLabelWidth(60, 0.35f, 0.01f);

		container->addChild(iconContainer);
		container->addChild(label);
		container->setID(fmt::format("{}-container", ID));

		container->setScale(0.8f);

		container->updateLayout();

		return container;
	}

	CCNode* createVerticalStatContainer(ZStringView ID) {
		auto container = CCNode::create();
		container->setLayout(SimpleColumnLayout::create()
			->setMainAxisScaling(AxisScaling::None)
			->setCrossAxisScaling(AxisScaling::None)
			->setMainAxisAlignment(MainAxisAlignment::Center)
		);
		container->setID(fmt::format("{}-container", ID));

		container->setContentSize({20, m_height});
		container->setAnchorPoint({1.f, 0.5f});
		container->setScale(0.8f);

		auto statsMenu = m_mainLayer->getChildByID("stats-menu");
		if (statsMenu) {
			Ref<CCNode> icon = statsMenu->getChildByID(fmt::format("{}-icon", ID));
			Ref<CCNode> label = statsMenu->getChildByID(fmt::format("{}-label", ID));

			if (!icon || !label) return nullptr;
			
			icon->removeFromParent();
			label->removeFromParent();

			static_cast<CCLabelBMFont*>(label.data())->limitLabelWidth(20, 0.35f, 0.01f);

			auto iconContainer = CCNode::create();
			iconContainer->setID(fmt::format("{}-icon-container", ID));
			iconContainer->setContentSize({20, 20});
			iconContainer->addChild(icon);

			icon->setPosition(iconContainer->getContentSize() / 2);

			container->addChild(iconContainer);
			container->addChild(label);
			
			container->updateLayout();
		}

		return container;
	}

    void loadFromScore(GJUserScore* score) {
		GJScoreCell::loadFromScore(score);
		addOnEnterCallback([this]  {
			if (shouldExclude()) return;
			setupChanges();
		});
	}

    void setupChanges() {
		float leftOffset = 0;

		if (m_score->m_playerRank != 0) {
			leftOffset = 15;
		}

		m_mainLayer->setContentSize({m_width, m_height});

		auto rankLabel = m_mainLayer->getChildByID("rank-label");
		if (rankLabel) {
			rankLabel->setPosition({5 + leftOffset, m_height / 2.f + 2});
			static_cast<CCLabelBMFont*>(rankLabel)->limitLabelWidth(24, 0.5f, 0.01f);
		}

		auto player = m_mainLayer->getChildByID("player-icon");
		if (!player) {
			player = m_mainLayer->getChildByID("cvolton.betterinfo/player-icon-menu");
		}

		if (player) {
			player->setPosition({35 + leftOffset, m_height / 2.f});
			player->setContentSize({0, 0});
			player->setScale(0.65f);
		}

		auto mainMenu = m_mainLayer->getChildByID("main-menu");
		if (mainMenu) {
			mainMenu->setContentSize({m_width - 100, m_height});
			mainMenu->ignoreAnchorPointForPosition(false);
			mainMenu->setAnchorPoint({0, 0.5});
			mainMenu->setScale(0.7f);
			mainMenu->setLayout(SimpleRowLayout::create()
				->setMainAxisAlignment(MainAxisAlignment::Start)
			);
			mainMenu->updateLayout();
			mainMenu->setPosition({53 + leftOffset, m_height / 2.f + 2});
		}

		auto statsContainer = CCNode::create();
		statsContainer->setContentSize({180, m_height - 10});
		statsContainer->setAnchorPoint({1.f, 0.5f});
		statsContainer->setPosition({m_width - 5, m_height / 2.f});
		statsContainer->setID("stats-container");
		statsContainer->setScale(0.9f);

		statsContainer->setLayout(RowLayout::create()
			->setCrossAxisOverflow(false)
			->setAutoScale(false)
			->setGrowCrossAxis(true)
			->setAxisAlignment(AxisAlignment::Start)
		);

		auto stars = createStatContainer("stars");
		auto moons = createStatContainer("moons");
		auto demons = createStatContainer("demons");
		auto userCoins = createStatContainer("user-coins");

		statsContainer->addChild(stars);
		statsContainer->addChild(moons);
		statsContainer->addChild(demons);
		statsContainer->addChild(createStatContainer("diamonds"));
		statsContainer->addChild(userCoins);
		statsContainer->addChild(createStatContainer("coins"));

		switch(m_score->m_leaderboardStat) {
			case LeaderboardStat::Stars:
				stars->setZOrder(-1);
				break;
			case LeaderboardStat::Moons:
				moons->setZOrder(-1);
				break;
			case LeaderboardStat::Demons:
				demons->setZOrder(-1);
				break;
			case LeaderboardStat::UserCoins:
				userCoins->setZOrder(-1);
				break;
		}

		statsContainer->updateLayout();
		m_mainLayer->addChild(statsContainer);

		auto verticalStatsContainer = CCNode::create();
		verticalStatsContainer->setContentSize({40, m_height});
		verticalStatsContainer->setAnchorPoint({1.f, 0.5f});
		verticalStatsContainer->setPosition({statsContainer->getPositionX() - statsContainer->getScaledContentWidth() - 2, m_height / 2});
		verticalStatsContainer->setID("vertical-stats-container");

		verticalStatsContainer->setLayout(SimpleRowLayout::create()
			->setGap(2.f)
			->setMainAxisAlignment(MainAxisAlignment::Start)
			->setMainAxisDirection(AxisDirection::RightToLeft)
			->setMainAxisScaling(AxisScaling::ScaleDown)
		);

		auto creatorPoints = createVerticalStatContainer("creator-points");
		if (creatorPoints) {
			verticalStatsContainer->addChild(creatorPoints);
		}

		auto sends = createVerticalStatContainer("sorkopiko.senddb/sends");
		if (sends) {
			verticalStatsContainer->addChild(sends);
		}
		
		Ref<CCNode> betterProgression = m_mainLayer->getChildByID("itzkiba.better_progression/tier-badge");
		if (betterProgression) {
			betterProgression->removeFromParentAndCleanup(false);
			betterProgression->setScale(0.37f);
			betterProgression->setZOrder(-1);
			verticalStatsContainer->addChild(betterProgression);
		}

		if (verticalStatsContainer->getChildrenCount() > 1) {
			if (mainMenu) {
				mainMenu->setScale(0.6f);
			}
		}

		verticalStatsContainer->updateLayout();
		m_mainLayer->addChild(verticalStatsContainer);
	}
};

class $modify(MyCustomListView, CustomListView) {

	static void onModify(auto& self) {
		(void) self.setHookPriorityPost("CustomListView::getListCell", Priority::VeryLate);
		(void) self.setHookPriorityPost("CustomListView::getCellHeight", Priority::VeryLate);
	}

    static CustomListView* create(cocos2d::CCArray* entries, TableViewCellDelegate* delegate, float height, float width, int page, BoomListType type, float y) {
		if (type == BoomListType::Score) {
			for (int i = entries->count() - 1; i >= 0; --i) {
				auto entry = static_cast<GJUserScore*>(entries->objectAtIndex(i));
				if (entry->m_accountID == 0) {
					entries->removeObjectAtIndex(i);
				}
			}
		}
		return CustomListView::create(entries, type, height, width);
	}

    TableViewCell* getListCell(char const* identifier) {
		auto ret = CustomListView::getListCell(identifier);
		if (shouldExclude()) return ret;

		if (m_type == BoomListType::Score) {
			ret->m_height = 35.f;
		}

		return ret;
	}

    static float getCellHeight(BoomListType type) {
		auto ret = CustomListView::getCellHeight(type);
		if (shouldExclude()) return ret;

		if (type == BoomListType::Score) {
			return 35.f;
		}
		return ret;
	}
};