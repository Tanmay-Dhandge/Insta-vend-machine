# Razorpay Integration Setup Instructions

## Step 1: Create a Razorpay Account  
1. Visit the [Razorpay website](https://razorpay.com).  
2. Click on the `Sign Up` button and create an account.  

## Step 2: Obtain API Keys  
1. After logging in to the Razorpay dashboard, navigate to `Settings` > `API Keys`.  
2. Click on `Generate Key` to create your API keys.  
3. Make sure to securely store your `Key ID` and `Key Secret`.  

## Step 3: Install Razorpay SDK  
Run the appropriate command to install the Razorpay SDK for your environment.  
For Node.js:  
```bash  
npm install razorpay  
```  
For PHP:  
```bash  
composer require razorpay/razorpay  
```  

## Step 4: Integrate Razorpay in Your Application  
### Frontend  
1. Include Razorpay Checkout script:  
```html  
<script src="https://checkout.razorpay.com/v1/checkout.js"></script>  
```  
2. Initialize Razorpay:  
```javascript  
var options = {  
    "key": "YOUR_KEY_ID",  
    "amount": "1000", // Amount in paise  
    "currency": "INR",  
    "name": "Your Company Name",  
    "description": "Test Transaction",  
    "handler": function (response){  
        alert(response.razorpay_payment_id);  
    },  
    "prefill": {  
        "name": "Your Name",  
        "email": "email@example.com",  
        "contact": "9999999999",  
    },  
    "theme": {  
        "color": "#F37254"  
    }  
};  

var rzp1 = new Razorpay(options);  

document.getElementById('razorpay-button').onclick = function(e){  
    rzp1.open();  
    e.preventDefault();  
}  
```  
### Backend  
Setup your backend server to verify payments via Razorpay webhook for security compliance.  
Refer to [Razorpay Webhooks Documentation](https://razorpay.com/docs/webhooks/) for detailed instructions.

## Step 5: Testing  
1. Use Razorpay's test mode to simulate transactions. Find test cards in the [Testing Documentation](https://razorpay.com/docs/payment-gateway/testing/#test-card-numbers).

## Step 6: Go Live  
Once you're confident that everything works fine in test mode, switch your API keys to live mode in the Razorpay dashboard.

## Additional Resources  
- [Razorpay API Documentation](https://razorpay.com/docs/api/)
- [Razorpay Checkout Documentation](https://razorpay.com/docs/payment-gateway/web-integration/checkout/)

Now you are ready to integrate Razorpay into your application!