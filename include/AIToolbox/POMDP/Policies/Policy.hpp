#ifndef AI_TOOLBOX_POMDP_POLICY_HEADER_FILE
#define AI_TOOLBOX_POMDP_POLICY_HEADER_FILE

#include <tuple>

#include <AIToolbox/POMDP/Types.hpp>
#include <AIToolbox/PolicyInterface.hpp>

namespace AIToolbox {
    namespace POMDP {
        /**
         * @brief This class represents a full policy.
         *
         * Building this object is expensive, so it should be done
         * mostly when it is known that the final solution won't
         * change again. Otherwise you may want to build a wrapper
         * around some data to extract the policy dynamically.
         */
        class Policy : public PolicyInterface<Belief> {
            public:
                /**
                 * @brief Basic constrctor.
                 *
                 * This constructor initializes the internal policy table so that
                 * each action in each state has the same probability of being
                 * chosen (random policy). This class guarantees that at any point
                 * the internal policy is a true probability distribution, i.e.
                 * for each state the sum of the probabilities of chosing an action
                 * sums up to 1.
                 *
                 * @param s The number of states of the world.
                 * @param a The number of actions available to the agent.
                 * @param o The number of possible observations the agent could make.
                 */
                Policy(size_t s, size_t a, size_t o);

                // This may be implemented, but probably not since it would be mostly impossible to convert
                // from a POMDP policy format to another.
                // Policy(const PolicyInterface<Belief> & p);

                /**
                 * @brief This function chooses a random action for belief b, following the policy distribution.
                 *
                 * Note that this will sample from the highest horizon that the Policy was computed for.
                 *
                 * @param b The sampled belief of the policy.
                 *
                 * @return The chosen action.
                 */
                virtual size_t sampleAction(const Belief & b) const override;

                /**
                 * @brief This function chooses a random action for belief b when horizon steps are missing, following the policy distribution.
                 *
                 * There are a couple of differences between this sampling function and the
                 * simpler version. The first one is that this function is actually able to sample from
                 * different timesteps, since this class is able to maintain a full policy tree over time.
                 *
                 * The second difference is that it returns two values. The first one is the requested action.
                 * The second return value is an id that allows the policy to compute more efficiently
                 * the sampled action during the next timestep, if provided to the Policy together with the
                 * obtained observation.
                 *
                 * @param b The sampled belief of the policy.
                 * @param horizon The requested horizon, meaning the number of timesteps missing until
                 * the end of the "episode". Both 1 and 0 return the same thing.
                 *
                 * @return A tuple containing the chosen action, plus an id useful to sample an action
                 * more efficiently at the next timestep, if required.
                 */
                std::tuple<size_t, size_t> sampleAction(const Belief & b, unsigned horizon) const;

                /**
                 * @brief This function chooses a random action after performing a sampled action and observing observation o, for a particular horizon.
                 *
                 * This sampling function is provided in case an already sampled action has been performed,
                 * an observation registered, and now a new action is needed for the next timestep.
                 *
                 * Note that this function works if and only if the horizon is going to be 1 (one) less
                 * than the value used for the previous sampling, otherwise anything could happen.
                 *
                 * To keep things simple, the id does not store internally the needed horizon value, and you
                 * are requested to keep track of it yourself.
                 *
                 * An example of usage for this function would be:
                 *
                 * ~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
                 * horizon = 3;
                 * // First sample
                 * auto result = sampleAction(belief, horizon);
                 * // We do the action, something happens, we get an observation.
                 * size_t observation = performAction(std::get<0>(result));
                 * --horizon;
                 * // We sample again, after reducing the horizon, with the previous id.
                 * result = sampleAction(std::get<1>(result), observation, horizon);
                 * ~~~~~~~~~~~~~~~~~~~~~~~
                 *
                 * @param id An id returned from a previous call of sampleAction.
                 * @param o The observation obtained after performing a previously sampled action.
                 * @param horizon The new horizon, equal to the old sampled horizon - 1.
                 *
                 * @return
                 */
                std::tuple<size_t, size_t> sampleAction(size_t id, size_t o, unsigned horizon) const;

                /**
                 * @brief This function returns the probability of taking the specified action in the specified belief.
                 *
                 * @param s The selected belief.
                 * @param a The selected action.
                 *
                 * @return The probability of taking the selected action in the specified belief.
                 */
                virtual double getActionProbability(const Belief & s, size_t a) const override;

            private:
        };
    }
}

#endif